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

#include "CollectionTypeResolver.h"

#include <QDir>
#include <QFileInfo>
#include <QString>

#include "libcute/collections/abstractcollection.h"
#include "libcute/collections/DirCollection.h"
#include "libcute/collections/IPodCollection.h"

/*!
 * This is our default constructor, which creates a new resolver object.
 */
CuteSyncCollectionTypeResolver::CuteSyncCollectionTypeResolver()
{
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CuteSyncCollectionTypeResolver::~CuteSyncCollectionTypeResolver()
{
}

/*!
 * This function provides our class's main functionality - namely, taking an input name and path and creating the
 * appropriate type of collection object to handle it. Note that this function doesn't necessarily need to be threaded;
 * we leave the actual calling of loadCollectionFromPath() to our user.
 *
 * Also note that we don't do any validity checking on the collection name; it is assumed that the caller did that
 * before calling this.
 *
 * /Also/ note that we do not retain ownership of the object returned; if we return non-NULL, it is YOUR responsibility
 * as the user to delete it as necessary.
 *
 * \param n The name for the new collection.
 * \param p The path to the collection.
 * \return A collection object, or NULL on error.
 */
CSAbstractCollection *CuteSyncCollectionTypeResolver::createCollection(const QString &n, const QString &p) const
{
	CSAbstractCollection *c = NULL;

	// Check if it's an iPod collection - this implies (path)/iPod_Control/iTunes/iTunesDB exists.

	QFileInfo itdb(QDir::cleanPath(p).append( QString("/iPod_Control/iTunes/iTunesDB").replace('/', QDir::separator()) ));
	if(itdb.exists())
		c = new CuteSyncIPodCollection(n);

	// If we didn't recognize it as anything special, treat it as simply a directory collection.

	if(c == NULL)
		c = new CuteSyncDirCollection(n);

	return c;
}
