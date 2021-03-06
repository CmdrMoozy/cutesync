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

#ifndef INCLUDE_CUTE_SYNC_DIALOGS_INSPECTOR_INSPECTOR_CONFIG_DIALOG_H
#define INCLUDE_CUTE_SYNC_DIALOGS_INSPECTOR_INSPECTOR_CONFIG_DIALOG_H

#include <QDialog>
#include <QAbstractListModel>
#include <QStringList>

#include "libcute/collections/abstractcollection.h"

class QGridLayout;
class QPushButton;
class QRadioButton;
class QGroupBox;

class CSDDSelectorWidget;
class CSDDSelectorModel;

/*!
 * \brief This class provides an interface for configuring our inspector widget.
 *
 * It allows the user to control the order we sort in, the columns we sort by,
 * and the columns we display.
 */
class CSInspectorConfigDialog : public QDialog
{
	Q_OBJECT

	public:
		CSInspectorConfigDialog(
			QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CSInspectorConfigDialog();

		void setDisplayDescriptor(
			const CSAbstractCollection::DisplayDescriptor &d);
		const CSAbstractCollection::DisplayDescriptor *
			getDisplayDescriptor() const;

	protected:
		virtual void showEvent(QShowEvent *e);

	private:
		CSAbstractCollection::DisplayDescriptor displayDescriptor;

		QGridLayout *layout;

		QGroupBox *sortGroupBox;
		QGridLayout *sortLayout;
		CSDDSelectorWidget *sortSelector;
		CSDDSelectorModel *sortModel;
		QRadioButton *ascendingButton;
		QRadioButton *descendingButton;

		QGroupBox *displayGroupBox;
		QGridLayout *displayLayout;
		CSDDSelectorWidget *displaySelector;
		CSDDSelectorModel *displayModel;

		QWidget *buttonsWidget;
		QGridLayout *buttonsLayout;
		QPushButton *okButton;
		QPushButton *applyButton;
		QPushButton *cancelButton;

		void createGUI();
		void updateDisplayDescriptor() const;

		void applyDisplayDescriptor();

	private Q_SLOTS:
		void doOk();
		void doApply();

	Q_SIGNALS:
		void accepted();
};

#endif
