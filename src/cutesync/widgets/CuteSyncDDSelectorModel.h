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

#ifndef INCLUDE_CUTE_SYNC_D_D_SELECTOR_MODEL_H
#define INCLUDE_CUTE_SYNC_D_D_SELECTOR_MODEL_H

#include <QAbstractListModel>

#include "libcute/collections/AbstractCollection.h"

/*!
 * \brief This class provides a model for our display descriptor selector widget.
 *
 * It more or less just maintains a list of columns (thereby storing their order), and whether or not
 * they are "enabled" for display or sorting.
 */
class CuteSyncDDSelectorModel : public QAbstractListModel
{
	public:
		CuteSyncDDSelectorModel(QObject *p = 0);
		virtual ~CuteSyncDDSelectorModel();

		QList<CuteSyncAbstractCollection::Column> getColumnList() const;

		void loadSortColumns(const CuteSyncAbstractCollection::DisplayDescriptor *d);
		void loadDisplayColumns(const CuteSyncAbstractCollection::DisplayDescriptor *d);

		virtual Qt::ItemFlags flags(const QModelIndex &i) const;
		virtual int rowCount(const QModelIndex &p = QModelIndex()) const;
		virtual QVariant data(const QModelIndex &i, int r = Qt::DisplayRole) const;
		virtual bool setData(const QModelIndex &i, const QVariant &v, int r = Qt::EditRole);
		virtual QVariant headerData(int s, Qt::Orientation o, int r = Qt::DisplayRole);

		void moveUp(int i);
		void moveDown(int i);

		virtual QModelIndex index(int r, int c, const QModelIndex &p = QModelIndex()) const;

	private:
		typedef struct SelectorItem
		{
			CuteSyncAbstractCollection::Column col;
			bool enabled;
		} SelectorItem;

		QList<SelectorItem> items;
};

#endif
