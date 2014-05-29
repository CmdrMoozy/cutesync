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

#ifndef INCLUDE_LIBCUTE_THREAD_COLLECTION_LOCKER_H
#define INCLUDE_LIBCUTE_THREAD_COLLECTION_LOCKER_H

class CuteSyncAbstractCollection;

/*!
 * \brief This class provides a mechanism for locking a collection.
 *
 * We lock the provided collection in our constructor, and then unlock it in our destructor. This
 * allows you to lock a collection in your function, and then not worry about unlocking it at every
 * possible exit point; we take care of that for you.
 */
class CuteSyncCollectionLocker
{
	public:
		CuteSyncCollectionLocker(const CuteSyncAbstractCollection *c);
		virtual ~CuteSyncCollectionLocker();

		const CuteSyncAbstractCollection *getCollection() const;

		void relock();
		void unlock();

	private:
		bool locked;
		const CuteSyncAbstractCollection *collection;
};

#endif
