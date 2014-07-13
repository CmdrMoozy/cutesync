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

#ifndef INCLUDE_CUTE_SYNC_WIDGETS_ICON_LIST_WIDGET_H
#define INCLUDE_CUTE_SYNC_WIDGETS_ICON_LIST_WIDGET_H

#include <QListView>

/*!
 * \brief This widget implements a simple list to display text/icon pairs.
 *
 * It functions similarly to a QListWidget, but has some added functionality.
 * For one, it ensures that we have one item per row and they are sized
 * appropriately according to our icon size. Second, it allows the user to use
 * a custom model - something QListWidget does not do. For your custom model,
 * you should probably have its data() function return valid information for
 * both DisplayRole and DecorationRole, so we can display your text and icon
 * respectively.
 */
class CSIconListWidget : public QListView
{
	public:
		CSIconListWidget(QWidget *p = 0);
		virtual ~CSIconListWidget();

		virtual int spacing() const;
		virtual void setSpacing(int s);

		virtual QSize iconSize() const;
		virtual void setIconSize(const QSize &s);

		bool hasSelection() const;
		QList<int> getSelectedRows() const;
		int getSelectedRow() const;
		void setSelectedRow(int r);

	protected:
		virtual void resizeEvent(QResizeEvent *e);

	private:
		int spc;
		QSize icons;

		int getItemWidth() const;
		int getItemHeight() const;

		void resizeItems();
};

#endif
