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

#ifndef INCLUDE_CUTE_SYNC_MAIN_MENU_BAR_H
#define INCLUDE_CUTE_SYNC_MAIN_MENU_BAR_H

#include <QMenuBar>

class QMenu;
class QAction;

class CSAboutDialog;

/*!
 * \brief This class implements a menu bar for our application.
 */
class CSMainMenuBar : public QMenuBar
{
	Q_OBJECT

	public:
		CSMainMenuBar(QWidget *p = 0);
		virtual ~CSMainMenuBar();

	private:
		CSAboutDialog *aboutDialog;

		QMenu *fileMenu;
		QAction *newCollectionAction;
		QAction *syncAction;
		QAction *removeCollectionAction;
		QAction *exitAction;

		QMenu *settingsMenu;
		QAction *resetSettingsAction;

		QMenu *helpMenu;
		QAction *aboutCuteSyncAction;
		QAction *aboutQtAction;

	private Q_SLOTS:
		void doAboutCuteSync();

	Q_SIGNALS:
		void newTriggered();
		void syncTriggered();
		void removeTriggered();
		void exitTriggered();
		void resetSettingsTriggered();
};

#endif
