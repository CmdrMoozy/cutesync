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

#include "ddselectormodel.h"

#include "libcute/defines.h"

/*!
 * This is our default constructor, which creates a new model object.
 *
 * \param p Our parent object.
 */
CSDDSelectorModel::CSDDSelectorModel(QObject *p)
	: QAbstractListModel(p)
{
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CSDDSelectorModel::~CSDDSelectorModel()
{
}

/*!
 * This function returns a list of the currently "enabled" columns, in the
 * correct order. It is up to the user to figure out how to apply this data.
 *
 * \return A list of enabled columns.
 */
QList<CuteSyncAbstractCollection::Column>
	CSDDSelectorModel::getColumnList() const
{
	QList<CuteSyncAbstractCollection::Column> colsList;

	for(int i = 0; i < rowCount(); ++i)
		if(items.at(i).enabled)
			colsList.append(items.at(i).col);

	return colsList;
}

/*!
 * This function loads sort columns from the given display descriptor. Columns
 * that are not in the descriptor's s_columns field are still added, but they
 * are initially "disabled" and in an arbitrary order.
 *
 * \param d The display descriptor to process.
 */
void CSDDSelectorModel::loadSortColumns(
	const CuteSyncAbstractCollection::DisplayDescriptor *d)
{
	items.clear();

	QList<CuteSyncAbstractCollection::Column> colsList;
	colsList.append(CuteSyncAbstractCollection::Artist);
	colsList.append(CuteSyncAbstractCollection::Album);
	colsList.append(CuteSyncAbstractCollection::Title);
	colsList.append(CuteSyncAbstractCollection::DiscNumber);
	colsList.append(CuteSyncAbstractCollection::TrackNumber);
	colsList.append(CuteSyncAbstractCollection::TrackCount);
	colsList.append(CuteSyncAbstractCollection::Length);
	colsList.append(CuteSyncAbstractCollection::Year);

	for(int i = 0; i < d->s_columns.count(); ++i)
	{
		CuteSyncAbstractCollection::Column col = d->s_columns.at(i);
		colsList.removeAll(col);

		items.append( {col, true} );
	}

	while(!colsList.isEmpty())
		items.append( {colsList.takeFirst(), false} );

	Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

/*!
 * This function loads display columns from the given display descriptor. Any
 * columns that are not in the descriptor's columns field are still added, but
 * they are initially "disabled" and in an arbitrary order.
 *
 * \param d The display descriptor to process.
 */
void CSDDSelectorModel::loadDisplayColumns(
	const CuteSyncAbstractCollection::DisplayDescriptor *d)
{
	items.clear();

	QList<CuteSyncAbstractCollection::Column> colsList;
	colsList.append(CuteSyncAbstractCollection::Artist);
	colsList.append(CuteSyncAbstractCollection::Album);
	colsList.append(CuteSyncAbstractCollection::Title);
	colsList.append(CuteSyncAbstractCollection::DiscNumber);
	colsList.append(CuteSyncAbstractCollection::TrackNumber);
	colsList.append(CuteSyncAbstractCollection::TrackCount);
	colsList.append(CuteSyncAbstractCollection::Length);
	colsList.append(CuteSyncAbstractCollection::Year);

	for(int i = 0; d->columns.contains(i); ++i)
	{
		CuteSyncAbstractCollection::Column col = d->columns.value(i);

		items.append( {col, true} );
		colsList.removeAll(col);
	}

	while(!colsList.isEmpty())
		items.append( {colsList.takeFirst(), false} );

	Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

/*!
 * We reimplement our superclass's flags() function to let views know that our
 * items are user-checkable.
 *
 * \param i The index to return flags for.
 * \return The flags for the specified index.
 */
Qt::ItemFlags CSDDSelectorModel::flags(const QModelIndex &i) const
{
	if(i.isValid())
	{
		return (Qt::ItemIsEnabled | Qt::ItemIsSelectable |
			Qt::ItemIsUserCheckable);
	}

	return QAbstractListModel::flags(i);
}

/*!
 * This function returns the number of rows currently in our model.
 *
 * \param p The parent index (unused).
 * \return The number of rows in the model.
 */
int CSDDSelectorModel::rowCount(const QModelIndex &UNUSED(p)) const
{
	return items.count();
}

/*!
 * This function returns the data for a specified item in the model. This can
 * be the display text, or its check state.
 *
 * \param i The index whose data we want.
 * \param r The display role of the data desired.
 * \return The data requested, as a QVariant.
 */
QVariant CSDDSelectorModel::data(const QModelIndex &i, int r) const
{
	switch(r)
	{
		case Qt::DisplayRole:
			return QVariant(
				CuteSyncAbstractCollection::getColumnPrettyName(
				items.at(i.row()).col));

		case Qt::CheckStateRole:
			return QVariant(items.at(i.row()).enabled
				? Qt::Checked
				: Qt::Unchecked);

		default:
			return QVariant(QVariant::Invalid);
	};
}

/*!
 * This function allows the user to /modify/ the data in our model (that is,
 * allows them to check or uncheck specific columns).
 *
 * \param i The index of the item whose data we are modifying.
 * \param v The new value this item will take on.
 * \param r The display role of the data given.
 * \return True if the item was modified, or false otherwise.
 */
bool CSDDSelectorModel::setData(const QModelIndex &i, const QVariant &v, int r)
{
	if(!i.isValid()) return false;

	switch(r)
	{
		case Qt::CheckStateRole:
			items[i.row()].enabled = v.value<bool>();
			Q_EMIT dataChanged(index(i.row(), 0),
				index(i.row(), 0));
			return true;

		default: return false;
	};
}

/*!
 * This function returns the header data for a specific part of our model. We
 * don't use headers, so this just returns an invalid QVariant.
 *
 * \param s The "section" of the header (i.e., row/column) (unused).
 * \param o The orientation (horizontal/vertical) of the header (unused).
 * \param r The display role of the header (unused).
 * \return The desired header data.
 */
QVariant CSDDSelectorModel::headerData(int UNUSED(s),
	Qt::Orientation UNUSED(o), int UNUSED(r))
{
	return QVariant(QVariant::Invalid);
}

/*!
 * This function moves a specified item up in our list. Note that if it is
 * already at the top, it is not moved. Also note that you should "probably"
 * update the selection on the view to reflect this change.
 *
 * \param i The index (row) of the item to be moved up.
 */
void CSDDSelectorModel::moveUp(int i)
{
	if((i-1) < 0) return;
	items.swap(i, i-1);
}

/*!
 * This function moves a specified item down in our list. Note that if it is
 * already at the bottom, it is not moved. Also note that you should "probably"
 * update the selection on the veiw to reflect this change.
 *
 * \param i The index (row) of the item to be moved down.
 */
void CSDDSelectorModel::moveDown(int i)
{
	if((i+1) >= rowCount()) return;
	items.swap(i, i+1);
}

/*!
 * We implement our superclass's index() function because it being private is
 * inconvenient. It should be mentioned, however, that you should not depend on
 * this index to remain sane or valid - use it once and get rid of it.
 *
 * \param r The desired row.
 * \param c The desired column.
 * \param p The parent index.
 * \return The specified model index.
 */
QModelIndex CSDDSelectorModel::index(int r, int c, const QModelIndex &p) const
{
	return QAbstractListModel::index(r, c, p);
}
