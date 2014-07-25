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

#include "mainmenubar.h"

#include <QMenu>
#include <QAction>
#include <QApplication>

#include "libcute/util/guiutils.h"
#include "cutesync/dialogs/aboutdialog.h"

/*!
 * This is our default constructor, which creates a new menu bar populated with
 * the actions for our application.
 *
 * \param p The menu bar's parent widget.
 */
CSMainMenuBar::CSMainMenuBar(QWidget *p)
	: QMenuBar(p)
{
	// Initialize our dialog.

	aboutDialog = new CSAboutDialog(this);

	// Initialize the file menu.

	fileMenu = addMenu(tr("&File"));

	newCollectionAction = fileMenu->addAction(tr("&New Collection..."));
	newCollectionAction->setIcon(CSGUIUtils::getIconFromTheme("list-add"));
	newCollectionAction->setShortcut(Qt::CTRL + Qt::Key_N);
	newCollectionAction->setStatusTip(tr("Create a new collection"));

	syncAction = fileMenu->addAction(tr("&Sync..."));
	syncAction->setIcon(
		CSGUIUtils::getIconFromTheme("sync-synchronizing"));
	syncAction->setShortcut(Qt::CTRL + Qt::Key_S);
	syncAction->setStatusTip(tr("Sync two collections"));

	removeCollectionAction = fileMenu->addAction(
		tr("&Remove Collection..."));
	removeCollectionAction->setIcon(
		CSGUIUtils::getIconFromTheme("list-remove"));
	removeCollectionAction->setStatusTip(
		tr("Remove the current collection"));

	fileMenu->addSeparator();

	exitAction = fileMenu->addAction(tr("E&xit"));
	exitAction->setIcon(CSGUIUtils::getIconFromTheme("application-exit"));
	exitAction->setStatusTip(tr("Exit CuteSync"));

	// Initialize the settings menu.

	settingsMenu = addMenu(tr("&Settings"));

	resetSettingsAction = settingsMenu->addAction(tr("Reset Settings"));
	resetSettingsAction->setStatusTip(
		tr("Reset all saved data to its default state"));

	// Initialize the help menu.

	helpMenu = addMenu(tr("&Help"));

	aboutCuteSyncAction = helpMenu->addAction(tr("&About CuteSync..."));
	aboutCuteSyncAction->setIcon(
		CSGUIUtils::getIconFromTheme("help-about"));
	aboutCuteSyncAction->setStatusTip(
		tr("Display CuteSync's about dialog"));

	aboutQtAction = helpMenu->addAction(tr("About &Qt..."));
	aboutQtAction->setIcon(CSGUIUtils::getIconFromTheme("help-about"));
	aboutQtAction->setStatusTip(
		tr("Display the Qt library's about dialog"));

	// Connect actions.

	QObject::connect(newCollectionAction, SIGNAL(triggered()),
		this, SIGNAL(newTriggered()));
	QObject::connect(syncAction, SIGNAL(triggered()),
		this, SIGNAL(syncTriggered()));
	QObject::connect(removeCollectionAction, SIGNAL(triggered()),
		this, SIGNAL(removeTriggered()));
	QObject::connect(exitAction, SIGNAL(triggered()),
		this, SIGNAL(exitTriggered()));
	QObject::connect(resetSettingsAction, SIGNAL(triggered()),
		this, SIGNAL(resetSettingsTriggered()));
	QObject::connect(aboutCuteSyncAction, SIGNAL(triggered()),
		this, SLOT(doAboutCuteSync()));
	QObject::connect(aboutQtAction, SIGNAL(triggered()),
		qApp, SLOT(aboutQt()));
}

/*!
 * This is our default destructor, which cleans up & destroys our menu bar.
 */
CSMainMenuBar::~CSMainMenuBar()
{
}

/*!
 * This function displays our about dialog as a result of the corresponding
 * menu/toolbar action being clicked.
 */
void CSMainMenuBar::doAboutCuteSync()
{ /* SLOT */

	aboutDialog->show();

}
