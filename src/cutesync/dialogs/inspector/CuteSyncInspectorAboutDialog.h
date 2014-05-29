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

#ifndef INCLUDE_CUTE_SYNC_INSPECTOR_ABOUT_DIALOG_H
#define INCLUDE_CUTE_SYNC_INSPECTOR_ABOUT_DIALOG_H

#include <QDialog>

class QGridLayout;
class QTextEdit;
class QPushButton;

/*!
 * \brief This dialog displays some information about a collection.
 *
 * It is inteded to be used by the CollectionInspector to display a collection's about text.
 */
class CuteSyncInspectorAboutDialog : public QDialog
{
	public:
		CuteSyncInspectorAboutDialog(QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CuteSyncInspectorAboutDialog();
	
		virtual void showEvent(QShowEvent *e);
	
		void setAboutText(const QString &t);
		
	private:
		QGridLayout *layout;
		QTextEdit *aboutText;
		QPushButton *closeButton;
};

#endif
