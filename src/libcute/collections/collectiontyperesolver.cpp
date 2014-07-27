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

#include "libcute/defines.h"
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
 * \param s Whether or not to remember the collection for next time.
 */
void CSCollectionTypeResolver::newCollection(const QString &n,
	const QString &p, bool s)
{ /* SLOT */

	CSAbstractCollection *c = NULL;

	// Create our new collection object.

	c = createCollection(n, p);

	// If we didn't create one, its type must be invalid.

	if(c == NULL)
	{
#pragma message "TODO - Errors need to be handled here by emitting a signal"
		return;
	}

	// Set our collection's save property.

	c->setSaveOnExit(s);

	// Connect the collection to our slots.

	QObject::connect(c, SIGNAL(jobStarted(const QString &, bool)),
		this, SLOT(doJobStarted(const QString &, bool)));
	QObject::connect(c, SIGNAL(progressLimitsUpdated(int, int)),
		this, SLOT(doProgressLimitsUpdated(int, int)));
	QObject::connect(c, SIGNAL(progressUpdated(int)),
		this, SLOT(doProgressUpdated(int)));
	QObject::connect(c, SIGNAL(jobFinished(const QString &)),
		this, SLOT(doJobFinished(const QString &)));

	/*
	 * The collection has been created, but we still need to load its data.
	 * Emit the signal indicating that the collection exists, but set it
	 * disabled (until it has been loaded).
	 */

	c->setEnabled(false);
	Q_EMIT(collectionCreated(c));

	// Try to load the collection from the path provided.

	c->loadCollectionFromPath(p);

}

/*!
 * This is a private utility function which creates a new collection using
 * the given collection details. We resolve the type of the collection, and
 * return an appropriate subclass of CSAbstractCollection.
 *
 * \param n The name of the collection.
 * \param p The path of the collection.
 */
CSAbstractCollection *CSCollectionTypeResolver::createCollection(
	const QString &n, const QString &p) const
{
	CSAbstractCollection *c = NULL;

	/*
	 * Check if it's an iPod collection - this implies
	 * (path)/iPod_Control/iTunes/iTunesDB exists.
	 */

	QFileInfo itdb(QDir::cleanPath(p).append(
		QString("/iPod_Control/iTunes/iTunesDB")
		.replace('/', QDir::separator())));

	if(itdb.exists())
		c = new CSIPodCollection(n);

	/*
	 * If we didn't recognize it as anything special, treat it as simply
	 * a directory collection.
	 */

	if(c == NULL)
		c = new CSDirCollection(n);

	return c;
}

/*!
 * This slot handles a new job being started by updating our job member, and
 * emitting an appropriate signal.
 *
 * \param j The string describing the job.
 * \param i Whether or not the job is interruptible.
 */
void CSCollectionTypeResolver::doJobStarted(const QString &j, bool i)
{ /* SLOT */

	CSAbstractCollection *s =
		dynamic_cast<CSAbstractCollection *>(sender());

	if(s != NULL)
		s->setEnabled(false);

	Q_EMIT jobStarted(j, i);

}

/*!
 * This slot handles the progress limits on a job being updated by emitting an
 * appropriate signal.
 *
 * \param min The new minimum limit.
 * \param max The new maximum limit.
 */
void CSCollectionTypeResolver::doProgressLimitsUpdated(int min, int max)
{ /* SLOT */

	Q_EMIT progressLimitsUpdated(min, max);

}

/*!
 * This slot handles the current progress of a job being updated by emitting an
 * appropriate signal.
 *
 * \param p The new current progress of the current job.
 */
void CSCollectionTypeResolver::doProgressUpdated(int p)
{ /* SLOT */

	Q_EMIT progressUpdated(p);

}

/*!
 * This slot handles the current job being finished by clearing out our job
 * member, and by emitting an appropriate signal.
 *
 * \param r The return text from the job (e.g., errors or whatever).
 */
void CSCollectionTypeResolver::doJobFinished(const QString &r)
{ /* SLOT */

	CSAbstractCollection *c =
		dynamic_cast<CSAbstractCollection *>(sender());

	if(c != NULL)
		c->setEnabled(true);

	Q_EMIT jobFinished(r);

}
