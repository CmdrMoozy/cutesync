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

#ifndef INCLUDE_LIBCUTE_THREAD_COLLECTION_THREAD_POOL_H
#define INCLUDE_LIBCUTE_THREAD_COLLECTION_THREAD_POOL_H

#include <QObject>

class QThread;

class CSCollectionTypeResolver;
class CSCollectionListItem;

class CSCollectionThreadPool : public QObject
{
	Q_OBJECT

	public:
		CSCollectionThreadPool(QObject *p = 0);
		virtual ~CSCollectionThreadPool();

	private:
		QThread *thread;
		CSCollectionTypeResolver *resolver;

	Q_SIGNALS:
		void collectionCreated(CSCollectionListItem *);

};

#endif
