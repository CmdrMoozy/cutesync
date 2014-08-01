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

#ifdef CUTESYNC_DEBUG
	#ifndef INCLUDE_CUTE_SYNC_DIALOGS_CREATE_IPOD_DIALOG_H
	#define INCLUDE_CUTE_SYNC_DIALOGS_CREATE_IPOD_DIALOG_H

	#include <QDialog>

	class QGridLayout;
	class QLabel;
	class QLineEdit;
	class QPushButton;

	/*!
	 * \brief This class implements a UI for creating new iPod structures.
	 */
	class CSCreateIPodDialog : public QDialog
	{
		Q_OBJECT

		public:
			CSCreateIPodDialog(QWidget *p = 0,
				Qt::WindowFlags f = 0);
			virtual ~CSCreateIPodDialog();

		protected:
			virtual void showEvent(QShowEvent *e);

		private:
			QGridLayout *layout;

			QLabel *nameLabel;
			QLineEdit *nameLineEdit;

			QLabel *pathLabel;
			QLineEdit *pathLineEdit;
			QPushButton *browseButton;

			QPushButton *createButton;
			QPushButton *cancelButton;

			void initializeGUI();

		private Q_SLOTS:
			void doBrowse();
			void doCreate();

		Q_SIGNALS:
			void ipodCreated(const QString &, const QString &);
	};

	#endif
#endif
