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
#include <QIcon>

#include "cutesync/dialogs/CuteSyncAboutDialog.h"

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

	aboutDialog = new CuteSyncAboutDialog(this);

	// Initialize the file menu.

	fileMenu = addMenu(tr("&File"));

	newCollectionAction = fileMenu->addAction(tr("&New Collection..."));
	newCollectionAction->setIcon(QIcon(":/icons/new.png"));
	newCollectionAction->setShortcut(Qt::CTRL + Qt::Key_N);
	newCollectionAction->setStatusTip(tr("Create a new collection"));

	syncAction = fileMenu->addAction(tr("&Sync..."));
	syncAction->setIcon(QIcon(":/icons/sync.png"));
	syncAction->setShortcut(Qt::CTRL + Qt::Key_S);
	syncAction->setStatusTip(tr("Sync two collections"));

	removeCollectionAction = fileMenu->addAction(
		tr("&Remove Collection..."));
	removeCollectionAction->setIcon(QIcon(":/icons/remove.png"));
	removeCollectionAction->setStatusTip(
		tr("Remove the current collection"));

	fileMenu->addSeparator();

	exitAction = fileMenu->addAction(tr("E&xit"));
	exitAction->setIcon(QIcon(":/icons/exit.png"));
	exitAction->setStatusTip(tr("Exit CuteSync"));

	// Initialize the settings menu.

	settingsMenu = addMenu(tr("&Settings"));

	resetSettingsAction = settingsMenu->addAction(tr("Reset Settings"));
	resetSettingsAction->setStatusTip(
		tr("Reset all saved data to its default state"));

	// Initialize the help menu.

	helpMenu = addMenu(tr("&Help"));

	aboutCuteSyncAction = helpMenu->addAction(tr("&About CuteSync..."));
	aboutCuteSyncAction->setIcon(QIcon(":/icons/about.png"));
	aboutCuteSyncAction->setStatusTip(
		tr("Display CuteSync's about dialog"));

	aboutQtAction = helpMenu->addAction(tr("About &Qt..."));
	aboutQtAction->setIcon(QIcon(":/icons/about.png"));
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
