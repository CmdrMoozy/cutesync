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

#include "CollectionLocker.h"

#include "libcute/collections/AbstractCollection.h"

/*!
 * This is our default constrcutor, which creates a new locker object and locks the given collection.
 *
 * \param c The collection to manage.
 */
CuteSyncCollectionLocker::CuteSyncCollectionLocker(const CuteSyncAbstractCollection *c)
	: locked(false), collection(c)
{
	relock();
}

/*!
 * This is our default destructor, which unlocks the collection we are managing.
 */
CuteSyncCollectionLocker::~CuteSyncCollectionLocker()
{
	unlock();
}

/*!
 * This function retrieves the collection our object is managing.
 *
 * \return The collection we are locking.
 */
const CuteSyncAbstractCollection *CuteSyncCollectionLocker::getCollection() const
{
	return collection;
}

/*!
 * This function relocks the collection if you have previously called unlock().
 */
void CuteSyncCollectionLocker::relock()
{
	if( (collection != NULL) && (!locked) )
	{
		collection->lock();
		locked = true;
	}
}

/*!
 * This function unlocks our collection.
 */
void CuteSyncCollectionLocker::unlock()
{
	if( (collection != NULL) && locked )
	{
		collection->unlock();
		locked = false;
	}
}
