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

#ifndef INCLUDE_CUTE_SYNC_NEW_COLLECTION_DIALOG_H
#define INCLUDE_CUTE_SYNC_NEW_COLLECTION_DIALOG_H

#include <QDialog>

class QShowEvent;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;

/*!
 * \brief This dialog gets user input to create a new collection.
 *
 * It collects things like the desired collection name, the path to the collection, what type of collection it is,
 * and whether or not it should be saved for next time our application is started.
 */
class CuteSyncNewCollectionDialog : public QDialog
{
	Q_OBJECT
	
	public:
		CuteSyncNewCollectionDialog(QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CuteSyncNewCollectionDialog();
	
		QString getName() const;
		QString getPath() const;
		bool getSave() const;
	
	protected:
		virtual void showEvent(QShowEvent *e);
	
	private:
		QGridLayout *layout;
		
		QLabel *nameLabel;
		QLineEdit *nameLineEdit;
		QLabel *pathLabel;
		QLineEdit *pathLineEdit;
		QPushButton *browseButton;
		QCheckBox *saveCheckBox;
	
		QWidget *buttonsWidget;
		QGridLayout *buttonsLayout;
		QPushButton *doItButton;
		QPushButton *cancelButton;
	
		void reset();
	
	private slots:
		void doBrowse();
		void doDoIt();
		void doCancel();
	
	signals:
		void accepted();
};

#endif
