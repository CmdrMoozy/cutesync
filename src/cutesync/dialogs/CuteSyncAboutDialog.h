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

#ifndef INCLUDE_CUTE_SYNC_ABOUT_DIALOG_H
#define INCLUDE_CUTE_SYNC_ABOUT_DIALOG_H

#include <QDialog>

class QGridLayout;
class QTabWidget;
class QLabel;
class QTextEdit;
class QPushButton;

/*!
 * \brief This class provides a simple "About" dialog for our application.
 *
 * It displays both our application's about text, as well as the GPL license text.
 */
class CuteSyncAboutDialog : public QDialog
{
	public:
		CuteSyncAboutDialog(QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CuteSyncAboutDialog();
	
	private:
		QGridLayout *layout;

		QTabWidget *aboutTabs;

		QWidget *cuteSyncTab;
		QGridLayout *cuteSyncTabLayout;
		QLabel *logoLabel;
		QLabel *headerLabel;
		QTextEdit *aboutText;

		QWidget *licenseTab;
		QGridLayout *licenseTabLayout;
		QTextEdit *licenseText;
		
		QPushButton *closeButton;
	
		void loadLicenseText();
};

#endif
