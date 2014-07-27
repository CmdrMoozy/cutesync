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

#include "mainwindow.h"

#ifdef CUTESYNC_DEBUG
	#include <iostream>
#endif

#include <QGridLayout>
#include <QProgressBar>
#include <QLabel>
#include <QSize>
#include <QSplitter>
#include <QCloseEvent>
#include <QMessageBox>

#include "libcute/defines.h"
#include "libcute/widgets/collectionmodel.h"
#include "cutesync/mainmenubar.h"
#include "cutesync/dialogs/newcollectiondialog.h"
#include "cutesync/dialogs/syncdialog.h"
#include "cutesync/settings/settingsmanager.h"
#include "cutesync/widgets/collectioninspector.h"
#include "cutesync/widgets/collectionlistwidget.h"

/*!
 * Create a new main window object with the given parent object and window
 * flags.
 *
 * \param p Our parent object.
 * \param f The window flags to use.
 */
CSMainWindow::CSMainWindow(QWidget *p, Qt::WindowFlags f)
	: QMainWindow(p, f)
{
	settingsManager = new CSSettingsManager(this);

	// Set some window properties.

	setWindowTitle(tr("CuteSync"));

	// Try to restore window geometry.
	restoreGeometry(settingsManager->getSetting(
		"window-geometry").value<QByteArray>());

	// Create our main GUI.

	createMenus();

	centralWidget = new QWidget(this);
	layout = new QGridLayout(centralWidget);

	collectionsSplitter = new QSplitter(centralWidget);

	collectionsListWidget = new CSCollectionListWidget(
		centralWidget);

	collectionsListModel = new CSCollectionModel();

	collectionsListWidget->setCollectionModel(collectionsListModel);

	collectionInspector = new CSCollectionInspector(
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
		SIGNAL(selectionChanged(CSAbstractCollection *)),
		collectionInspector,
		SLOT(setCollection(CSAbstractCollection *)));

	QObject::connect(collectionsListModel,
		SIGNAL(jobStarted(const QString &, bool)), this,
		SLOT(doWorkerJobStarted(const QString &, bool)));
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
		SIGNAL(startReload(CSAbstractCollection *)),
		collectionsListModel,
		SLOT(reloadCollection(CSAbstractCollection *)));
	QObject::connect(this,
		SIGNAL(startRefresh(CSAbstractCollection *)),
		collectionsListModel,
		SLOT(refreshCollection(CSAbstractCollection *)));
	QObject::connect(this,
		SIGNAL(startSync(CSAbstractCollection *,
		CSAbstractCollection *)), collectionsListModel,
		SLOT(syncCollections(CSAbstractCollection *,
		CSAbstractCollection *)));

	QObject::connect(collectionInspector,
		SIGNAL(reloadRequested(CSAbstractCollection *)),
		this, SIGNAL(startReload(CSAbstractCollection *)));
	QObject::connect(collectionInspector,
		SIGNAL(refreshRequested(CSAbstractCollection *)),
		this, SIGNAL(startRefresh(CSAbstractCollection *)));

	// Finally, restore our window state.

	restoreState(settingsManager->getSetting("window-state")
		.value<QByteArray>());

	// Restore other stuff from settings.

	Q_EMIT startUnserialize(settingsManager->getSetting(
		"saved-collections").value< QList<QVariant> >());
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSMainWindow::~CSMainWindow()
{
	delete collectionsListModel;
}

/*!
 * This function handles a window close event.
 *
 * \param e The close event we are handling.
 */
void CSMainWindow::closeEvent(QCloseEvent *e)
{
	// Stop our worker thread(s).

	collectionsListModel->stopGracefully();

	// Save everything.

	settingsManager->setSetting("display-descriptor",
		QVariant(CSAbstractCollection::serializeDisplayDescriptor(
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
 * This function creates the menus our window has and populates them with our
 * actions.
 */
void CSMainWindow::createMenus()
{
	mainMenuBar = new CSMainMenuBar(this);

	setMenuBar(mainMenuBar);

	QObject::connect(mainMenuBar, SIGNAL(newTriggered()),
		this, SLOT(doNewCollection()));
	QObject::connect(mainMenuBar, SIGNAL(syncTriggered()),
		this, SLOT(doSync()));
	QObject::connect(mainMenuBar, SIGNAL(removeTriggered()),
		this, SLOT(doRemoveCollection()));
	QObject::connect(mainMenuBar, SIGNAL(exitTriggered()),
		this, SLOT(close()));
	QObject::connect(mainMenuBar, SIGNAL(resetSettingsTriggered()),
		this, SLOT(doResetSettings()));
}

/*!
 * This function creates and connects the dialogs our window makes use of.
 */
void CSMainWindow::createDialogs()
{
	newCollectionDialog = new CSNewCollectionDialog(this);
	syncDialog = new CSSyncDialog(collectionsListModel, this);

	QObject::connect(newCollectionDialog, SIGNAL(accepted()),
		this, SLOT(doNewCollectionAccepted()));
	QObject::connect(syncDialog, SIGNAL(accepted(
		CSAbstractCollection *, CSAbstractCollection *)),
		this, SLOT(doSyncAccepted(CSAbstractCollection *,
		CSAbstractCollection *)));
}

/*!
 * This function handles our new collection dialog being accepted by going
 * ahead and trying to add the new collection.
 */
void CSMainWindow::doNewCollectionAccepted()
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
void CSMainWindow::doSyncAccepted(
	CSAbstractCollection *s, CSAbstractCollection *d)
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
void CSMainWindow::doNewCollection()
{ /* SLOT */
	newCollectionDialog->show();
}

/*!
 * This function displays our sync dialog as a result of the corresponding
 * menu/toolbar action being clicked.
 */
void CSMainWindow::doSync()
{ /* SLOT */

	syncDialog->show();

}

/*!
 * This function tries to remove the current collection, if any. The collection
 * will be flushed, so any outstanding I/O will be performed before it is
 */
void CSMainWindow::doRemoveCollection()
{ /* SLOT */

//TODO - We need to stop the collection from doing anything else.
	/*
	int r = collectionsListWidget->getSelectedRow();
	if(r == -1)
		return;

	CSAbstractCollection *c = collectionsListModel->collectionAt(r);
	if(c == NULL)
		return;

	c->setEnabled(false);

	/
	 * Collections shouldn't be active; active collections are not
	 * clickable, but check anyway.
	 /

	if(c->isActive())
	{
		QMessageBox::critical(this, tr("Error Removing Collection"),
			tr("The collection specified is still active. Wait for it to finish to remove it."));

		return;
	}

	c->flush();
	collectionsListModel->removeCollectionAt(r);
	*/
}

/*!
 * This function resets all of our settings to their default values.
 */
void CSMainWindow::doResetSettings()
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
 * This function handles a new job being started by our worker by setting the
 * appropriate text on our status label.
 *
 * \param j A string describing the job that was just started.
 * \param i Whether or not the job is interruptible (UNUSED).
 */
void CSMainWindow::doWorkerJobStarted(const QString &j, bool UNUSED(i))
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
void CSMainWindow::doWorkerProgressLimitsUpdated(int min, int max)
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
void CSMainWindow::doWorkerProgressUpdated(int p)
{ /* SLOT */

	taskProgressBar->setValue(p);

}

/*!
 * This function handles a job being completed by our worker by resetting our
 * progress bar and status label back to their default idle status.
 *
 * \param r The result text - error messages, status, etc.
 */
void CSMainWindow::doWorkerJobFinished(const QString &r)
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
void CSMainWindow::doSettingChanged(
	const QString &UNUSED(k), const QVariant &UNUSED(v))
{

}
