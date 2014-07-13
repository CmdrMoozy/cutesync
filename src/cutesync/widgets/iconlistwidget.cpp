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

#include "iconlistwidget.h"

#include <QResizeEvent>
#include <QFontMetrics>

/*!
 * This is our default constructor, which creates a new widget with the given
 * parent. Note that this widget has a default spacing of 5, and a default icon
 * size of 32 by 32. Change this if this is not what you want.
 *
 * \param p Our parent widget.
 */
CSIconListWidget::CSIconListWidget(QWidget *p)
	: QListView(p)
{
	setSpacing(5);
	setIconSize(QSize(32,32));
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSIconListWidget::~CSIconListWidget()
{
}

/*!
 * This function retrieves the amount of spacing we currently have between
 * items.
 *
 * \return Our current spacing.
 */
int CSIconListWidget::spacing() const
{
	return spc;
}

/*!
 * This function sets the amount of spacing we should have between items in the
 * list.
 *
 * \param s Our new spacing.
 */
void CSIconListWidget::setSpacing(int s)
{
	spc = s;
	QListView::setSpacing(s);
	resizeItems();
}

/*!
 * This function retrieves the icon size we are currently using for items in
 * our list.
 *
 * \return Our current icon size.
 */
QSize CSIconListWidget::iconSize() const
{
	return icons;
}

/*!
 * This function sets the icon size we should use to draw the items in our
 * list.
 *
 * \param s Our new icon size.
 */
void CSIconListWidget::setIconSize(const QSize &s)
{
	icons = s;
	QListView::setIconSize(s);
	resizeItems();
}

/*!
 * This function returns whether or not our widget currently has a selection.
 * True means at least one item is selected, and false means nothing is
 * selected.
 *
 * \return Whether or not we have a selection.
 */
bool CSIconListWidget::hasSelection() const
{
	return (selectedIndexes().length() > 0);
}

/*!
 * This function returns a list of all the selected row indices, in the same
 * order returned by our internal selectedIndexes() function. If nothing is
 * selected, an empty list is returned instead.
 */
QList<int> CSIconListWidget::getSelectedRows() const
{
	QModelIndexList idx = selectedIndexes();
	QList<int> r;

	while(!idx.isEmpty())
		r.append(idx.takeFirst().row());

	return r;
}

/*!
 * This function returns the index of the currently selected row. If no
 * selection is currently active, or if multiple selection is enabled, this
 * function will fail and returns -1 instead.
 *
 * \return The currently selected row.
 */
int CSIconListWidget::getSelectedRow() const
{
	if(selectionMode() == QAbstractItemView::SingleSelection)
	{
		QModelIndexList idx = selectedIndexes();

		if(!idx.isEmpty())
			return idx.first().row();
	}

	return -1;
}

/*!
 * This is a convenience function that sets a specific row as being selected.
 * One can accomplish the same thing using setCurrentIndex() and index() on the
 * model, but this is far easier.
 *
 * \param r The desired row.
 */
void CSIconListWidget::setSelectedRow(int r)
{
	if(model() == NULL) return;
	setCurrentIndex(model()->index(r, 0));
}

/*!
 * We override our superclass's resize event, so we can make sure the items in
 * our list are all going to be sized correctly.
 *
 * \param e The event we are handling.
 */
void CSIconListWidget::resizeEvent(QResizeEvent *e)
{
	resizeItems();
	e->accept();
}

/*!
 * This function returns the width items in our list should have. This is
 * generally the same as the width of our widget.
 *
 * \return The width of a list item.
 */
int CSIconListWidget::getItemWidth() const
{
	return width();
}

/*!
 * This function returns the height items in our list should have. This is
 * generally the larger of our icon size and our font size, plus the spacing
 * our items have around them.
 *
 * \return The height of a list item.
 */
int CSIconListWidget::getItemHeight() const
{
	return qMax(iconSize().height(), QFontMetrics(font()).lineSpacing()) +
		(spacing() * 2);
}

/*!
 * This is a convenience function that updates things so our items will be
 * drawn correctly. This should be called, e.g., after our widget is resized,
 * or some size property is changed.
 */
void CSIconListWidget::resizeItems()
{
	setGridSize(QSize(getItemWidth(), getItemHeight()));
}
