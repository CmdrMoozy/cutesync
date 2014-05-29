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

#ifndef INCLUDE_CUTE_SYNC_INSPECTOR_CONFIG_DIALOG_H
#define INCLUDE_CUTE_SYNC_INSPECTOR_CONFIG_DIALOG_H

#include <QDialog>
#include <QAbstractListModel>
#include <QStringList>

#include "libcute/collections/CuteSyncAbstractCollection.h"

class QGridLayout;
class QPushButton;
class QRadioButton;
class QGroupBox;

class CuteSyncDDSelectorWidget;
class CuteSyncDDSelectorModel;

/*!
 * \brief This class provides an interface for configuring our CollectionInspector widget.
 *
 * It allows the user to control the order we sort in, the columns we sort by, and the columns we
 * display.
 */
class CuteSyncInspectorConfigDialog : public QDialog
{
	Q_OBJECT

	public:
		CuteSyncInspectorConfigDialog(QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CuteSyncInspectorConfigDialog();

		void setDisplayDescriptor(const CuteSyncAbstractCollection::DisplayDescriptor &d);
		const CuteSyncAbstractCollection::DisplayDescriptor *getDisplayDescriptor() const;

	protected:
		virtual void showEvent(QShowEvent *e);

	private:
		CuteSyncAbstractCollection::DisplayDescriptor displayDescriptor;

		QGridLayout *layout;

		QGroupBox *sortGroupBox;
		QGridLayout *sortLayout;
		CuteSyncDDSelectorWidget *sortSelector;
		CuteSyncDDSelectorModel *sortModel;
		QRadioButton *ascendingButton;
		QRadioButton *descendingButton;

		QGroupBox *displayGroupBox;
		QGridLayout *displayLayout;
		CuteSyncDDSelectorWidget *displaySelector;
		CuteSyncDDSelectorModel *displayModel;

		QWidget *buttonsWidget;
		QGridLayout *buttonsLayout;
		QPushButton *okButton;
		QPushButton *applyButton;
		QPushButton *cancelButton;

		void createGUI();
		void updateDisplayDescriptor() const;

		void applyDisplayDescriptor();

	private slots:
		void doOk();
		void doApply();

	signals:
		void accepted();
};

#endif
