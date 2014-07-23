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

#include "collectiontyperesolver.h"

#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QByteArray>

#include "libcute/collections/abstractcollection.h"
#include "libcute/collections/dircollection.h"
#include "libcute/collections/ipodcollection.h"
#include "libcute/widgets/collectionlistitem.h"

/*!
 * This is our default constructor, which creates a new resolver object.
 */
CSCollectionTypeResolver::CSCollectionTypeResolver(QObject *p)
	: QObject(p)
{
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSCollectionTypeResolver::~CSCollectionTypeResolver()
{
}

/*!
 * This function provides our class's main functionality - namely, taking an
 * input name and path and creating the appropriate type of collection object
 * to handle it. Note that this function should probably not be run in the
 * event dispatch thread; we load the collection from the path given, which can
 * take a nontrivial amount of time.
 *
 * Also note that we don't do any validity checking on the collection name; it
 * is assumed that the caller did that before calling this function.
 *
 * The returned collection will have this object as a parent, so deleting this
 * object will also delete any collections it created. The new collection will
 * also have the same thread affinity as this object.
 *
 * The new collection, instead of being returned directly, will be given to our
 * caller (and potentially others) via a signal emission once the collection is
 * loaded. This means that this operation can be done in a different thread
 * from our caller.
 *
 * \param n The name for the new collection.
 * \param p The path to the collection.
 */
void CSCollectionTypeResolver::createCollection(
	const QString &n, const QString &p) const
{ /* SLOT */

	/*
	CSAbstractCollection *c = NULL;

	/
	 * Check if it's an iPod collection - this implies
	 * (path)/iPod_Control/iTunes/iTunesDB exists.
	 /

	QFileInfo itdb(QDir::cleanPath(p).append(
		QString("/iPod_Control/iTunes/iTunesDB")
		.replace('/', QDir::separator())));

	if(itdb.exists())
		c = new CSIPodCollection(n);

	/
	 * If we didn't recognize it as anything special, treat it as simply
	 * a directory collection.
	 /

	if(c == NULL)
		c = new CSDirCollection(n);

	return c;
	*/

}

void CSCollectionTypeResolver::unserializeCollection(const QByteArray &d) const
{ /* SLOT */



}
