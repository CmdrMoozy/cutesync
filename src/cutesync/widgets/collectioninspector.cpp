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

#include "collectioninspector.h"

#include <cmath>

#include <QGridLayout>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QProgressBar>
#include <QTimerEvent>

#include "libcute/collections/abstractcollection.h"
#include "libcute/util/guiutils.h"
#include "libcute/util/systemutils.h"
#include "cutesync/dialogs/inspector/inspectoraboutdialog.h"
#include "cutesync/dialogs/inspector/inspectorcollectionconfigdialog.h"
#include "cutesync/dialogs/inspector/inspectorconfigdialog.h"
#include "cutesync/settings/settingsmanager.h"

/*!
 * This is our default constructor, which creates a new inspector widget.
 *
 * \param p Our parent widget.
 */
CSCollectionInspector::CSCollectionInspector(
	CSSettingsManager *s, QWidget *p)
	: QWidget(p), settingsManager(s), collection(NULL)
{
	createGUI();
	createDialogs();

	loadDisplayDescriptor();

	startTimer(5000);

	// Connect settings manager.
	QObject::connect(settingsManager,
		SIGNAL(settingChanged(const QString &, const QVariant &)),
		this,
		SLOT(doSettingChanged(const QString &, const QVariant &)));
}

/*!
 * This is a convenience constructor, which creates a new inspector widget and
 * immediately sets it to display the contents of the provided collection.
 *
 * \param c The collection to display.
 * \param p Our parent widget.
 */
CSCollectionInspector::CSCollectionInspector(
	CSSettingsManager *s, CSAbstractCollection *c, QWidget *p)
	: QWidget(p), settingsManager(s), collection(NULL)
{
	createGUI();
	createDialogs();

	loadDisplayDescriptor();

	setCollection(c);

	startTimer(5000);

	// Connect settings manager.
	QObject::connect(settingsManager,
		SIGNAL(settingChanged(const QString &, const QVariant &)),
		this,
		SLOT(doSettingChanged(const QString &, const QVariant &)));
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CSCollectionInspector::~CSCollectionInspector()
{
}

/*!
 * This function clears our current collection, making it so we are not
 * displaying anything. This function is just shorthand for setting the
 * collection to NULL.
 */
void CSCollectionInspector::clearCollection()
{
	setCollection(NULL);
}

/*!
 * This function returns a copy of the display descriptor our inspector is
 * currently using.
 *
 * \return Our current display descriptor.
 */
CSAbstractCollection::DisplayDescriptor
	CSCollectionInspector::getDisplayDescriptor() const
{
	return displayDescriptor;
}

/*!
 * This function returns the collection we are currently displaying, or NULL if
 * one hasn't been given to us yet.
 *
 * \return Our current collection.
 */
CSAbstractCollection *CSCollectionInspector::getCollection() const
{
	return collection;
}

/*!
 * This slot sets the collection we should be displaying.
 *
 * \param c The collection we will display.
 */
void CSCollectionInspector::setCollection(CSAbstractCollection *c)
{ /* SLOT */

	// Disconnect our old collection.
	if(collection != NULL)
		collection->disconnect(this);

	// Load up the new collection.

	collection = c;

	if(collection != NULL)
	{
		collection->setDisplayDescriptor(&displayDescriptor);
		collection->sort();

		collectionViewer->setModel(collection);
		collectionViewer->resizeColumnsToContents();

		spaceUsedProgressBar->setRange(0, 100);
		spaceUsedProgressBar->setValue(static_cast<int>(
			floor(CSSystemUtils::getDeviceUsedPercent(
			collection->getMountPoint().toStdString())) ));

		// Connect the new collection.
		QObject::connect(collection, SIGNAL(enabledChanged()),
			this, SLOT(doCollectionEnabledChanged()));
		QObject::connect(collection, SIGNAL(contentsChanged()),
			this, SLOT(doCollectionContentsChanged()));
	}
	else
	{
		spaceUsedProgressBar->setRange(0, 0);
		spaceUsedProgressBar->setValue(0);

		collectionViewer->setModel(NULL);
	}
}

/*!
 * Some elements of our GUI may update without us knowing directly about it;
 * this class utilizes QObject's built-in timer functionality to update some
 * elements of the GUI periodically via this function.
 *
 * \param e The event we are handling.
 */
void CSCollectionInspector::timerEvent(QTimerEvent *e)
{
	if(collection != NULL)
	{
		// Update our disk space used progress bar.
		spaceUsedProgressBar->setValue(static_cast<int>(
			floor(CSSystemUtils::getDeviceUsedPercent(
			collection->getMountPoint().toStdString())) ));
	}

	e->accept();
}

/*!
 * This is a helper function that creates our GUI for us; this is so we don't
 * have to copypasta the code to do so between our multiple constructors.
 */
void CSCollectionInspector::createGUI()
{
	layout = new QGridLayout(this);

	actionsList = new QWidget(this);
	actionsLayout = new QGridLayout(actionsList);

	aboutButton = new QPushButton(tr("About..."), actionsList);

	refreshButton = new QPushButton(tr("Refresh"), actionsList);

	configButton = new QPushButton(tr("Options..."), actionsList);

	sortButton = new QPushButton(
		CSGUIUtils::getIconFromTheme("preferences-other"),
		QString(), actionsList);
	sortButton->setToolTip(tr("Configure Inspector"));

	actionsLayout->addWidget( aboutButton,   0, 0 );
	actionsLayout->addWidget( refreshButton, 0, 1 );
	actionsLayout->addWidget( configButton,  0, 2 );
	actionsLayout->addWidget( sortButton,    0, 4 );
	actionsLayout->setColumnStretch(3, 1);
	actionsList->setLayout(actionsLayout);

	collectionViewer = new QTableView(this);
	collectionViewer->setSelectionBehavior(QAbstractItemView::SelectRows);
	collectionViewer->setSelectionMode(
		QAbstractItemView::ExtendedSelection);
	collectionViewer->setSortingEnabled(false);

	spaceUsedLabel = new QLabel(tr("Disk Space Used:"), this);
	spaceUsedProgressBar = new QProgressBar(this);

	layout->addWidget(actionsList, 0, 0, 1, 2);
	layout->addWidget(collectionViewer, 1, 0, 1, 2);
	layout->addWidget(spaceUsedLabel, 2, 0, 1, 1);
	layout->addWidget(spaceUsedProgressBar, 2, 1, 1, 1);
	layout->setColumnStretch(1, 1);
	layout->setRowStretch(1, 1);
	setLayout(layout);

	QObject::connect(aboutButton, SIGNAL(clicked()),
		this, SLOT(doAboutDevice()));
	QObject::connect(refreshButton, SIGNAL(clicked()),
		this, SLOT(doRefresh()));
	QObject::connect(configButton, SIGNAL(clicked()),
		this, SLOT(doConfig()));
	QObject::connect(sortButton, SIGNAL(clicked()),
		this, SLOT(doSort()));
}

/*!
 * This is a helper function that initializes and connects our widget's various
 * dialogs.
 */
void CSCollectionInspector::createDialogs()
{
	aboutDialog = new CSInspectorAboutDialog(this);
	configDialog = new CSInspectorCollectionConfigDialog(this);
	sortDialog = new CSInspectorConfigDialog(this);

	QObject::connect(sortDialog, SIGNAL(accepted()),
		this, SLOT(doSortAccepted()));
}

/*!
 * This function loads the display descriptor that has been stored by our
 * settings manager.
 */
void CSCollectionInspector::loadDisplayDescriptor()
{
	displayDescriptor =
		CSAbstractCollection::unserializeDisplayDescriptor(
		settingsManager->getSetting("display-descriptor")
		.value<QByteArray>());
}

/*!
 * This function handles our configuration dialog being accepted by updating
 * our display descriptor with the new data, and then applying those changes to
 * the collection we are currently viewing (if any).
 */
void CSCollectionInspector::doSortAccepted()
{ /* SLOT */
	displayDescriptor = (*sortDialog->getDisplayDescriptor());

	if(collection != NULL)
	{
		collection->setDisplayDescriptor(&displayDescriptor);
		collectionViewer->resizeColumnsToContents();
	}
}

/*!
 * This function handles our "About..." button being clicked by displaying the
 * about dialog for the collection we are currently displaying, if any.
 */
void CSCollectionInspector::doAboutDevice()
{ /* SLOT */

	if(collection != NULL)
	{
		aboutDialog->setAboutText(collection->getAboutText());
		aboutDialog->show();
	}

}

/*!
 * This function handles our refresh button by refreshing the current
 * collection, if any.
 */
void CSCollectionInspector::doRefresh()
{ /* SLOT */

#pragma message "TODO - We need to re-activate the collection automatically after refreshing"

	if(collection != NULL)
		Q_EMIT refreshRequested(collection);


}

/*!
 * This function handles our collection configuration button being clicked by
 * displaying our collection configuration dialog.
 */
void CSCollectionInspector::doConfig()
{ /* SLOT */

	if(collection != NULL)
	{
		configDialog->setWidget(
			collection->getConfigurationWidget());
		configDialog->show();
	}

}

/*!
 * This function handles our sort configuration button being clicked by
 * displaying our configuration dialog.
 */
void CSCollectionInspector::doSort()
{ /* SLOT */
	if(!sortDialog->isVisible())
		sortDialog->setDisplayDescriptor(displayDescriptor);

	sortDialog->show();
}

/*!
 * This function handles a collection's enabled state being changed by either
 * displaying it if it is now enabled, or unloading it if it is NOT enabled.
 * This is to prevent editing/viewing of collections whose state is not
 * "valid."
 */
void CSCollectionInspector::doCollectionEnabledChanged()
{ /* SLOT */

	if(!collection->isEnabled())
		clearCollection();

}

/*!
 * This function handles our collection's contents being modified, which means
 * we need to refresh our GUI appropriately.
 */
void CSCollectionInspector::doCollectionContentsChanged()
{ /* SLOT */

	/*
	 * Because tracks have been added/removed/modified, we need to re-sort
	 * the collection.
	 */

	collection->sort();

}

/*!
 * This function handles a setting being changed, and takes appropriate action.
 * For instance, when the display descriptor is changed, we update our GUI
 * accordingly.
 *
 * \param k The key of the setting that was changed.
 * \param v The setting's new value.
 */
void CSCollectionInspector::doSettingChanged(
	const QString &k, const QVariant &v)
{ /* SLOT */

	if(k == "display-descriptor")
	{
		displayDescriptor =CSAbstractCollection
			::unserializeDisplayDescriptor(v.value<QByteArray>());

		if(collection != NULL)
		{
			collection->setDisplayDescriptor(&displayDescriptor);
			collectionViewer->resizeColumnsToContents();
		}
	}
}
