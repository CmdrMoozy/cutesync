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

#ifndef INCLUDE_CUTE_SYNC_DIALOGS_SYNC_DIALOG_H
#define INCLUDE_CUTE_SYNC_DIALOGS_SYNC_DIALOG_H

#include <QDialog>
#include <QString>
#include <QList>

class QGridLayout;
class QLabel;
class QComboBox;
class QPushButton;

class CuteSyncAbstractCollection;
class CSCollectionModel;

/*!
 * \brief This dialog allows the user to start a synchronization.
 *
 * It is designed to provide both generic synchronization options as well as
 * destination-specific options.
 */
class CSSyncDialog : public QDialog
{
	Q_OBJECT

	public:
		CSSyncDialog(CSCollectionModel *c,
			QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CSSyncDialog();

	protected:
		virtual void showEvent(QShowEvent *e);

	private:
		CSCollectionModel *collections;

		QGridLayout *layout;

		QLabel *sourceLabel;
		QComboBox *sourceComboBox;
		QLabel *destinationLabel;
		QComboBox *destinationComboBox;

		QWidget *buttonsWidget;
		QGridLayout *buttonsLayout;
		QPushButton *doItButton;
		QPushButton *cancelButton;

		void createGUI();
		void updateGUI();

	private Q_SLOTS:
		void doDoIt();

	Q_SIGNALS:
		void accepted();
		void accepted(CuteSyncAbstractCollection *,
			CuteSyncAbstractCollection *);
};

#endif
