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

#ifndef INCLUDE_CUTE_SYNC_WIDGETS_COLLECTION_DELEGATE_H
#define INCLUDE_CUTE_SYNC_WIDGETS_COLLECTION_DELEGATE_H

#include <QStyledItemDelegate>

class CSCollectionModel;

/*!
 * \brief This class provides extra rendering for CuteSyncCollectionListWidget.
 *
 * We allow items in the list to be "disabled," so they are greyed out and
 * cannot be clicked, but are still visible in the list. This is used when some
 * action is being performed on a collection that makes it unavailable for more
 * actions to be queued up.
 */
class CSCollectionDelegate : public QStyledItemDelegate
{
	public:
		CSCollectionDelegate(QObject *p = 0);
		virtual ~CSCollectionDelegate();

		const CSCollectionModel *getModel() const;
		void setModel(const CSCollectionModel *m);

		virtual void paint(QPainter *p, const QStyleOptionViewItem &o,
			const QModelIndex &i) const;

	private:
		const CSCollectionModel *model;
};

#endif
