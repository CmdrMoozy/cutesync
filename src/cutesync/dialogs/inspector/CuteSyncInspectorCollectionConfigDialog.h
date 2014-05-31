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

#ifndef INCLUDE_CUTE_SYNC_INSPECTOR_COLLECTION_CONFIG_DIALOG_H
#define INCLUDE_CUTE_SYNC_INSPECTOR_COLLECTION_CONFIG_DIALOG_H

#include <QDialog>

class QGridLayout;
class QPushButton;

class CuteSyncAbstractCollectionConfigWidget;

/*!
 * \brief This dialog provides collection configuration support.
 *
 * We utilize a collection's built-in configuration widget in order to allow
 * the user to change its options.
 */
class CuteSyncInspectorCollectionConfigDialog : public QDialog
{
	Q_OBJECT

	public:
		CuteSyncInspectorCollectionConfigDialog(
			QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CuteSyncInspectorCollectionConfigDialog();

		void setWidget(CuteSyncAbstractCollectionConfigWidget *w);

	protected:
		virtual void showEvent(QShowEvent *e);

	private:
		QGridLayout *layout;

		CuteSyncAbstractCollectionConfigWidget *widget;

		QWidget *buttonsWidget;
		QGridLayout *buttonsLayout;
		QPushButton *okButton;
		QPushButton *applyButton;
		QPushButton *cancelButton;

	private Q_SLOTS:
		void doOk();
		void doApply();
};

#endif
