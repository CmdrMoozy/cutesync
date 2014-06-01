/*
 * CuteSync - A media library management and synchronization application.
 * Copyright (C) 2011 Axel Rasmussen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CuteSyncMainWindow.h"

#ifdef CUTESYNC_DEBUG
	#include <iostream>
#endif

#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QGridLayout>
#include <QProgressBar>
#include <QLabel>
#include <QSize>
#include <QMenuBar>
#include <QIcon>
#include <QApplication>
#include <QSplitter>
#include <QCloseEvent>
#include <QLocalServer>
#include <QMessageBox>

#include "libcute/Defines.h"
#include "cutesync/dialogs/CuteSyncAboutDialog.h"
#include "cutesync/dialogs/CuteSyncNewCollectionDialog.h"
#include "cutesync/dialogs/CuteSyncSyncDialog.h"
#include "cutesync/settings/CuteSyncSettingsManager.h"
#include "cutesync/widgets/CuteSyncCollectionInspector.h"
#include "cutesync/widgets/CuteSyncCollectionListWidget.h"
#include "libcute/widgets/CollectionModel.h"

/*!
 * Create a new main window object with the given parent object and window
 * flags.
 *
 * \param p Our parent object.
 * \param f The window flags to use.
 */
CuteSyncMainWindow::CuteSyncMainWindow(CuteSyncSettingsManager *s,
	QWidget *p, Qt::WindowFlags f)
	: QMainWindow(p, f), settingsManager(s)
{
	/*
	 * We are going to instantiate a local server and listen for other
	 * instances of CuteSync to start up.
	 */

	sappServer = new QLocalServer(this);
	if(!sappServer->listen(CUTE_SYNC_APP_UUID))
	{
		/*
		 * If we got to this point, it means that we didn't get a
		 * response from the person that created our socket, but it is
		 * still present for some reason. Try removing it; if that
		 * doesn't work, crash.
		 */

		QLocalServer::removeServer(CUTE_SYNC_APP_UUID);
		if(!sappServer->listen(CUTE_SYNC_APP_UUID))
		{
#ifdef CUTESYNC_DEBUG
			std::cout << "FATAL: Failed to initialize single-app server! Terminating!\n";
			std::cout << "QLocalServer reports: " <<
				sappServer->errorString()
				.toLatin1().data() << "\n";
#endif

			qApp->quit();
		}
	}

	QObject::connect(sappServer, SIGNAL(newConnection()),
		this, SLOT(doDuplicateInstanceDetected()));

	// Set some window properties.

	setWindowTitle(tr("CuteSync"));

	// Try to restore window geometry.
	restoreGeometry(settingsManager->getSetting(
		"window-geometry").value<QByteArray>());

	// Create some of our supporting members.

	createActions();
	createMenus();
	createToolBars();

	// Create our main GUI.

	centralWidget = new QWidget(this);
	layout = new QGridLayout(centralWidget);

	collectionsSplitter = new QSplitter(centralWidget);

	collectionsListWidget = new CuteSyncCollectionListWidget(
		centralWidget);

	collectionsListModel = new CuteSyncCollectionModel();
	collectionsListModel->moveToThread(&worker);

	collectionsListWidget->setCollectionModel(collectionsListModel);

	collectionInspector = new CuteSyncCollectionInspector(
		settingsManager, centralWidget);

	collectionsSplitter->addWidget(collectionsListWidget);
	collectionsSplitter->addWidget(collectionInspector);

	taskProgressBar = new QProgressBar(centralWidget);
	taskLabel = new QLabel(tr("Idle."), centralWidget);

	layout->addWidget( collectionsSplitter, 0, 0, 1, 2 );
	layout->addWidget( taskProgressBar,     1, 0, 1, 1 );
	layout->addWidget( taskLabel,           1, 1, 1, 1 );
	layout->setColumnStretch(1, 1);
	layout->setRowStretch(0, 1);
	centralWidget->setLayout(layout);

	setCentralWidget(centralWidget);

	// Create our dialogs, now that GUI elements have been initialized.

	createDialogs();

	// Connect signals/slots.

	QObject::connect(settingsManager,
		SIGNAL(settingChanged(const QString &, const QVariant &)),
		this,
		SLOT(doSettingChanged(const QString &, const QVariant &)));

	QObject::connect(collectionsListWidget,
		SIGNAL(selectionChanged(CuteSyncAbstractCollection *)),
		collectionInspector,
		SLOT(setCollection(CuteSyncAbstractCollection *)));

	QObject::connect(collectionsListModel,
		SIGNAL(jobStarted(const QString &)), this,
		SLOT(doWorkerJobStarted(const QString &)));
	QObject::connect(collectionsListModel,
		SIGNAL(progressLimitsUpdated(int, int)), this,
		SLOT(doWorkerProgressLimitsUpdated(int, int)));
	QObject::connect(collectionsListModel,
		SIGNAL(progressUpdated(int)), this,
		SLOT(doWorkerProgressUpdated(int)));
	QObject::connect(collectionsListModel,
		SIGNAL(jobFinished(const QString &)), this,
		SLOT(doWorkerJobFinished(const QString &)));

	QObject::connect(this,
		SIGNAL(startUnserialize(const QList<QVariant> &)),
		collectionsListModel,
		SLOT(loadSerializedList(const QList<QVariant> &)));
	QObject::connect(this,
		SIGNAL(startNew(const QString &, const QString &, bool)),
		collectionsListModel,
		SLOT(newCollection(const QString &, const QString &, bool)));
	QObject::connect(this,
		SIGNAL(startReload(CuteSyncAbstractCollection *)),
		collectionsListModel,
		SLOT(reloadCollection(CuteSyncAbstractCollection *)));
	QObject::connect(this,
		SIGNAL(startRefresh(CuteSyncAbstractCollection *)),
		collectionsListModel,
		SLOT(refreshCollection(CuteSyncAbstractCollection *)));
	QObject::connect(this,
		SIGNAL(startSync(CuteSyncAbstractCollection *,
		CuteSyncAbstractCollection *)), collectionsListModel,
		SLOT(syncCollections(CuteSyncAbstractCollection *,
		CuteSyncAbstractCollection *)));

	QObject::connect(collectionInspector,
		SIGNAL(reloadRequested(CuteSyncAbstractCollection *)),
		this, SIGNAL(startReload(CuteSyncAbstractCollection *)));
	QObject::connect(collectionInspector,
		SIGNAL(refreshRequested(CuteSyncAbstractCollection *)),
		this, SIGNAL(startRefresh(CuteSyncAbstractCollection *)));

	// Start our worker thread rolling.

	worker.start();

	// Finally, restore our window state.

	restoreState(settingsManager->getSetting("window-state")
		.value<QByteArray>());

	// Restore other stuff from settings.

	Q_EMIT startUnserialize(settingsManager->getSetting(
		"saved-collections").value< QList<QVariant> >());
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 *
 * MAKE SURE THE WORKER IS STOPPED BEFORE OUR DESTRUCTOR IS CALLED.
 */
CuteSyncMainWindow::~CuteSyncMainWindow()
{
	delete collectionsListModel;
}

/*!
 * This function handles a close event. We need to safely terminate the worker
 * thread; this means:
 *
 *     - If it is idle, just terminate it gracefully.
 *     - If it's doing something but that something is interruptible, go ahead
 *       and terminate.
 *     - Otherwise, ask the user if they really want to exit and do the
 *       appropriate thing.
 *
 * \param e The close event we are handling.
 */
void CuteSyncMainWindow::closeEvent(QCloseEvent *e)
{
	// Stop our worker as gracefully as possible.

	if(!collectionsListModel->isIdle())
	{
		if(!collectionsListModel->isInterruptAdvised())
		{
			switch(QMessageBox::question(this,
				tr("Interrupt action?"),
				tr("CuteSync is currently performing an action that shouldn't be interrupted. Do you want to edit anyway?"),
				QMessageBox::Yes | QMessageBox::No))
			{
				case QMessageBox::Yes:
					break;

				default:
					e->ignore();
					return;
			}
		}

		collectionsListModel->setInterrupted(true);
	}

	worker.quit();
	worker.wait();

	// Save everything.
	settingsManager->setSetting("display-descriptor",
		QVariant(CuteSyncAbstractCollection::serializeDisplayDescriptor(
		collectionInspector->getDisplayDescriptor())));
	settingsManager->setSetting("window-geometry",
		QVariant(saveGeometry()));
	settingsManager->setSetting("window-state", QVariant(saveState()));
	settingsManager->setSetting("saved-collections",
		QVariant(collectionsListModel->getSerializedList()));

	// Exit!

	e->accept();
}

/*!
 * This function creates and connects the QAction objects our window uses for,
 * e.g., menus and toolbars.
 */
void CuteSyncMainWindow::createActions()
{
	newCollectionAction = new QAction(QIcon(":/icons/new.png"),
		tr("&New Collection..."), this);
	newCollectionAction->setShortcut(Qt::CTRL + Qt::Key_N);
	newCollectionAction->setStatusTip(tr("Create a new collection"));

	syncAction = new QAction(QIcon(":/icons/sync.png"),
		tr("&Sync..."), this);
	syncAction->setShortcut(Qt::CTRL + Qt::Key_S);
	syncAction->setStatusTip(tr("Sync two collections"));

	removeCollectionAction = new QAction(QIcon(":/icons/remove.png"),
		tr("&Remove Collection..."), this);
	removeCollectionAction->setStatusTip(
		tr("Remove the current collection"));

	exitAction = new QAction(QIcon(":/icons/exit.png"), tr("E&xit"), this);
	exitAction->setStatusTip(tr("Exit CuteSync"));

	resetSettingsAction = new QAction(
		tr("Reset Settings to Defaults"), this);
	resetSettingsAction->setStatusTip(
		tr("Reset all saved data to its default state"));

	aboutCuteSyncAction = new QAction(QIcon(":/icons/about.png"),
		tr("&About CuteSync..."), this);
	aboutCuteSyncAction->setStatusTip(
		tr("Display CuteSync's about dialog"));

	aboutQtAction = new QAction(QIcon(":/icons/about.png"),
		tr("About &Qt..."), this);
	aboutQtAction->setStatusTip(
		tr("Display the Qt library's about dialog"));

	QObject::connect(newCollectionAction, SIGNAL(triggered()),
		this, SLOT(doNewCollection()));
	QObject::connect(syncAction, SIGNAL(triggered()),
		this, SLOT(doSync()));
	QObject::connect(removeCollectionAction, SIGNAL(triggered()),
		this, SLOT(doRemoveCollection()));
	QObject::connect(exitAction, SIGNAL(triggered()),
		this, SLOT(close()));
	QObject::connect(resetSettingsAction, SIGNAL(triggered()),
		this, SLOT(doResetSettings()));
	QObject::connect(aboutCuteSyncAction, SIGNAL(triggered()),
		this, SLOT(doAboutCuteSync()));
	QObject::connect(aboutQtAction, SIGNAL(triggered()),
		qApp, SLOT(aboutQt()));
}

/*!
 * This function creates the menus our window has and populates them with our
 * actions.
 */
void CuteSyncMainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newCollectionAction);
	fileMenu->addAction(syncAction);
	fileMenu->addAction(removeCollectionAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	settingsMenu = menuBar()->addMenu(tr("&Settings"));
	settingsMenu->addAction(resetSettingsAction);

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutCuteSyncAction);
	helpMenu->addAction(aboutQtAction);
}

/*!
 * This function creates the toolbars our window has and populates them with
 * our actions.
 */
void CuteSyncMainWindow::createToolBars()
{
	mainToolBar = addToolBar(tr("Main Tool Bar"));
	mainToolBar->setObjectName("mainToolBar");

	mainToolBar->addAction(newCollectionAction);
	mainToolBar->addAction(syncAction);
	mainToolBar->addAction(removeCollectionAction);
}

/*!
 * This function creates and connects the dialogs our window makes use of.
 */
void CuteSyncMainWindow::createDialogs()
{
	newCollectionDialog = new CuteSyncNewCollectionDialog(this);
	syncDialog = new CuteSyncSyncDialog(collectionsListModel, this);
	aboutDialog = new CuteSyncAboutDialog(this);

	QObject::connect(newCollectionDialog, SIGNAL(accepted()),
		this, SLOT(doNewCollectionAccepted()));
	QObject::connect(syncDialog, SIGNAL(accepted(
		CuteSyncAbstractCollection *, CuteSyncAbstractCollection *)),
		this, SLOT(doSyncAccepted(CuteSyncAbstractCollection *,
		CuteSyncAbstractCollection *)));
}

/*!
 * This slot is used in conjunction with our QLocalServer to detect duplicate
 * instances of CuteSync being started. In theory, if another instance of
 * CuteSync is started while we are running, it will notify us, and this slot
 * will be called.
 *
 * Note that the behavior of this slot is operating-system dependent; on "most"
 * platforms it will simply make our task bar item flash to alert the user that
 * we were pinged, but on some platforms this may actually bring the window to
 * the front.
 */
void CuteSyncMainWindow::doDuplicateInstanceDetected()
{ /* SLOT */
	activateWindow();
}

/*!
 * This function handles our new collection dialog being accepted by going
 * ahead and trying to add the new collection.
 */
void CuteSyncMainWindow::doNewCollectionAccepted()
{ /* SLOT */

	Q_EMIT startNew(newCollectionDialog->getName(),
		newCollectionDialog->getPath(),
		newCollectionDialog->getSave());

}

/*!
 * This function handles our sync dialog being accepted by going ahead and
 * doing the sync operation.
 *
 * \param s The source collection.
 * \param d The destination collection.
 */
void CuteSyncMainWindow::doSyncAccepted(
	CuteSyncAbstractCollection *s, CuteSyncAbstractCollection *d)
{ /* SLOT */

	// Make sure our collections are valid-ish.

	if( (s == NULL) || (d == NULL) )
	{
		QMessageBox::critical(this, tr("Sync Error"),
			tr("You must select both a source and a destination collection to sync."));

		return;
	}

	if(s == d)
	{
		QMessageBox::critical(this, tr("Sync Error"),
			tr("You cannot sync a collection with itself."));

		return;
	}

	// Sync our collections!

	Q_EMIT startSync(s, d);

}

/*!
 * This function displays our new collection dialog as a result of the
 * corresponding menu/toolbar action being clicked.
 */
void CuteSyncMainWindow::doNewCollection()
{ /* SLOT */
	newCollectionDialog->show();
}

/*!
 * This function displays our sync dialog as a result of the corresponding
 * menu/toolbar action being clicked.
 */
void CuteSyncMainWindow::doSync()
{ /* SLOT */

	syncDialog->show();

}

/*!
 * This function tries to remove the current collection, if any. The collection
 * will be flushed, so any outstanding I/O will be performed before it is
 */
void CuteSyncMainWindow::doRemoveCollection()
{ /* SLOT */

	int r = collectionsListWidget->getSelectedRow();
	if(r == -1)
		return;

	CuteSyncAbstractCollection *c = collectionsListModel->collectionAt(r);
	if(c == NULL)
		return;

	c->setEnabled(false);

	/*
	 * Collections shouldn't be active; active collections are not
	 * clickable, but check anyway.
	 */

	if(c->isActive())
	{
		QMessageBox::critical(this, tr("Error Removing Collection"),
			tr("The collection specified is still active. Wait for it to finish to remove it."));

		return;
	}

	c->flush();
	collectionsListModel->removeCollectionAt(r);
}

/*!
 * This function resets all of our settings to their default values.
 */
void CuteSyncMainWindow::doResetSettings()
{ /* SLOT */

	switch(QMessageBox::question(this, tr("Reset Settings"),
		tr("Really reset all settings to default values (including saved collections)?"),
		QMessageBox::Yes | QMessageBox::No))
	{
		case QMessageBox::Yes:
			settingsManager->resetDefaults();
			break;

		default:
			return;
	}

}

/*!
 * This function displays our about dialog as a result of the corresponding
 * menu/toolbar action being clicked.
 */
void CuteSyncMainWindow::doAboutCuteSync()
{ /* SLOT */
	aboutDialog->show();
}

/*!
 * This function handles a new job being started by our worker by setting the
 * appropriate text on our status label.
 *
 * \param j A string describing the job that was just started.
 */
void CuteSyncMainWindow::doWorkerJobStarted(const QString &j)
{ /* SLOT */
	taskLabel->setText(j);
}

/*!
 * This function handles the progress limits of a job being updated by our
 * worker by setting the appropriate limits on our progress bar, and resetting
 * the progress to 0.
 *
 * \param min The minimum half of the new limit.
 * \param max The maximum half of the new limit.
 */
void CuteSyncMainWindow::doWorkerProgressLimitsUpdated(int min, int max)
{ /* SLOT */
	taskProgressBar->setRange(min, max);
	taskProgressBar->setValue(0);
}

/*!
 * This function handles the progress of a job being updated by our worker by
 * setting the appropriate progress on our progress bar.
 *
 * \param p The new progress value.
 */
void CuteSyncMainWindow::doWorkerProgressUpdated(int p)
{ /* SLOT */
	taskProgressBar->setValue(p);
}

/*!
 * This function handles a job being completed by our worker by resetting our
 * progress bar and status label back to their default idle status.
 *
 * \param r The result text - error messages, status, etc.
 */
void CuteSyncMainWindow::doWorkerJobFinished(const QString &r)
{ /* SLOT */
	if(!r.isEmpty())
	{
		QMessageBox::information(this,
			tr("Job Finished"), tr("Job finished!"));
	}

	taskProgressBar->reset();
	taskLabel->setText(tr("Idle."));
}

/*!
 * This function detects when a setting has been changed, and takes the
 * appropriate action.
 *
 * \param k The key of the setting that was changed.
 * \param v The new value for this setting.
 */
void CuteSyncMainWindow::doSettingChanged(
	const QString &UNUSED(k), const QVariant &UNUSED(v))
{

}
