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

#ifndef INCLUDE_CUTE_SYNC_D_D_SELECTOR_WIDGET_H
#define INCLUDE_CUTE_SYNC_D_D_SELECTOR_WIDGET_H

#include <QWidget>

class QGridLayout;
class QListView;
class QPushButton;

class CuteSyncDDSelectorModel;

/*!
 * \brief This widget provides a way to view or modify a display descriptor.
 *
 * We can do either display columns or sort columns. These should be loaded
 * into a DDSelectorModel and then given to this class. We allow the user to
 * enable/disable columns, and to change the order of the columns via up/down
 * buttons.
 */
class CuteSyncDDSelectorWidget : public QWidget
{
	Q_OBJECT

	public:
		CuteSyncDDSelectorWidget(QWidget *p = 0);
		virtual ~CuteSyncDDSelectorWidget();

		void setModel(CuteSyncDDSelectorModel *m);
		CuteSyncDDSelectorModel *model() const;

	private:
		QGridLayout *layout;
		QListView *view;
		QPushButton *upButton;
		QPushButton *downButton;

	private Q_SLOTS:
		void doUpClicked();
		void doDownClicked();
};

#endif
