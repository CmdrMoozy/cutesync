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

#ifndef INCLUDE_CUTE_SYNC_COLLECTION_LIST_WIDGET
#define INCLUDE_CUTE_SYNC_COLLECTION_LIST_WIDGET

#include "cutesync/widgets/CuteSyncIconListWidget.h"

class CuteSyncAbstractCollection;
class CuteSyncCollectionModel;
class CuteSyncCollectionDelegate;

/*!
 * \brief This class provides a list widget for storing collections.
 *
 * We have ownership of all of the collections, so you don't need to worry about freeing them later. We also
 * handle creating the correct type of object based on the collection type.
 */
class CuteSyncCollectionListWidget : public CuteSyncIconListWidget
{
	Q_OBJECT

	public:
		CuteSyncCollectionListWidget(QWidget *p = 0);
		virtual ~CuteSyncCollectionListWidget();

		virtual QAbstractItemModel *model() const;
		void setCollectionModel(CuteSyncCollectionModel *m);

	private:
		CuteSyncCollectionDelegate *delegate;
		CuteSyncCollectionModel *mdl;

		virtual void setModel(QAbstractItemModel *m);

	private slots:
		void doCollectionClicked(const QModelIndex &i);
		void doRowEnabledChanged(const QModelIndex &i);

	signals:
		void selectionChanged(CuteSyncAbstractCollection *);
};

#endif
