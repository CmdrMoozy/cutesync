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

#include "CuteSyncCollectionModel.h"

#ifdef CUTESYNC_DEBUG
	#include <iostream>
#endif

#include <QIcon>
#include <QDataStream>
#include <QMutex>
#include <QMutexLocker>
#include <QFileInfo>
#include <QMessageBox>

#include "CuteSyncDefines.h"
#include "collections/CuteSyncAbstractCollection.h"

/*!
 * This is our default constructor, which creates a new model object with the given parent.
 *
 * \param p Our parent object.
 */
CuteSyncCollectionModel::CuteSyncCollectionModel(QObject *p)
	: QAbstractListModel(p), currentJob(NULL)
{
	mutex = new QMutex(QMutex::Recursive);
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CuteSyncCollectionModel::~CuteSyncCollectionModel()
{
	clear();
	delete mutex;
}

/*!
 * This function returns the number of rows our model contains. This is another way of calling
 * count().
 *
 * \return The number of rows in our model.
 */
int CuteSyncCollectionModel::rowCount(const QModelIndex &UNUSED(p)) const
{
	return count();
}

/*!
 * This function returns the display data for a given cell in the model. This function handles both the
 * display text of a collection (its name) and the icon it uses (via getDisplayIcon() on the collection
 * superclass).
 *
 * \param i The index in our model. Since we only have one column, we only use the row portion of this.
 * \param r The display role for the desired data.
 */
QVariant CuteSyncCollectionModel::data(const QModelIndex &i, int r) const
{
	switch(r)
	{
		case Qt::DisplayRole:
			{
				CuteSyncAbstractCollection *c = collectionAt(i.row());
				if(c != NULL)
					return QVariant(c->getName());
				else
					return QVariant(QVariant::Invalid);
			}

		case Qt::DecorationRole:
			{
				CuteSyncAbstractCollection *c = collectionAt(i.row());
				if(c != NULL)
					return QVariant(c->getDisplayIcon());
				else
					return QVariant(QVariant::Invalid);
			}

		default:
			return QVariant(QVariant::Invalid);
	};
}

/*!
 * This function returns the desired header data. Because our model doesn't have column/row headers by
 * design, we just return an invalid QVariant.
 *
 * \param s The section (row/col) for the data.
 * \param o The orientation of the data.
 * \param r The display role of the data.
 */
QVariant CuteSyncCollectionModel::headerData(int UNUSED(s), Qt::Orientation UNUSED(o), int UNUSED(r)) const
{
	return QVariant(QVariant::Invalid);
}

/*!
 * This function clears our items list, deleting all items present.
 */
void CuteSyncCollectionModel::clear()
{
	CuteSyncAbstractCollection *c;
	while(!itemList.isEmpty())
	{
		c = itemList.takeFirst();
		delete c;
	}
}

/*!
 * This function returns the number of items currently in our model. This value is also used by rowCount() to
 * provide the same information to the view using us as a model.
 *
 * \return The number of items in our list.
 */
int CuteSyncCollectionModel::count() const
{
	return itemList.count();
}

/*!
 * This function returns the collection pointer that is in our model at the given position (i.e., row). If
 * the position provided is out-of-bounds, NULL is returned instead.
 *
 * \param i The index (row) of the desired collection.
 * \return The desired collection.
 */
CuteSyncAbstractCollection *CuteSyncCollectionModel::collectionAt(int i) const
{
	if( (i < 0) || (i >= count()) ) return NULL;
	return itemList.at(i);
}

/*!
 * This function returns a pointer to the collection in our model with the given name. Note that the string
 * comparison we do is case-sensitive.
 *
 * \param n The name to search for.
 * \return A pointer to the desired collection, or NULL if it is not found.
 */
CuteSyncAbstractCollection *CuteSyncCollectionModel::collectionFromName(const QString &n) const
{
	for(int i = 0; i < count(); ++i)
		if(collectionAt(i)->getName() == n)
			return collectionAt(i);

	return NULL;
}

/*!
 * This function removes the collection at the given index in our model. Note that this deletes the collection,
 * since we take ownership of anything added to us; be sure not to try and use a different pointer to the same
 * collection after you call this.
 *
 * Note that it is probably unnecessary to check if the collection is currently working, since active collections
 * are not clickable in order to be removed. It is, however, up to the caller to perform this check if it is
 * indeed needed for whatever reason.
 *
 * \param i The index (row) of the collection to be removed.
 */
void CuteSyncCollectionModel::removeCollectionAt(int i)
{
	if( (i < 0) || (i >= count()) ) return;

	emit beginResetModel();

	itemList.at(i)->disconnect();
	CuteSyncAbstractCollection *c = itemList.takeAt(i);
	c->deleteLater();

	emit endResetModel();
}

/*!
 * This function appends the given collection to our model. Note that from here on our WE ARE TAKING
 * OWNERSHIP OF THE COLLECTION - this means that we will delete it if need be. Note that this is probably
 * the only function you should be calling to add new items to the model, unless you have a really
 * good reason for calling something else.
 *
 * \param c The collection to add to our model.
 */
void CuteSyncCollectionModel::appendCollection(CuteSyncAbstractCollection *c)
{
	if( (c != NULL) && (!itemList.contains(c)) )
	{
		QObject::connect(c, SIGNAL(enabledChanged()),
			this, SLOT(doCollectionEnabledChanged()));

		itemList.append(c);

		emit dataChanged(createIndex(itemList.count()-1, 0), createIndex(itemList.count()-1, 0));
	}
}

/*!
 * This function returns a list of the names of all of the collections in our model. This could be used, e.g., to
 * populate a combo box or something.
 *
 * \return A list of the names in our model.
 */
QList<QString> CuteSyncCollectionModel::getCollectionNameList() const
{
	QList<QString> r;

	for(int i = 0; i < count(); ++i)
		r.append(collectionAt(i)->getName());

	return r;
}

/*!
 * This function tests whether or not all of the collections in our model are currently idle.
 *
 * \return True if we are totally idle, or false if a collection is doing something.
 */
bool CuteSyncCollectionModel::isIdle() const
{
	return (currentJob == NULL);
}

/*!
 * This function tests if the current job is easily interruptible or not.
 *
 * \return True if we can interrupt the current job, or false if we probably shouldn't.
 */
bool CuteSyncCollectionModel::isInterruptAdvised() const
{
	if(currentJob != NULL)
		return currentJob->isInterruptible();

	return true;
}

/*!
 * This function sets the interrupt flag on the current job, if any. When a job is interrupted, it
 * will try to halt itself as quickly as possible. Note that this DOES NOT take into account whether
 * it is considered interruptible.
 *
 * \param i
 */
void CuteSyncCollectionModel::setInterrupted(bool i)
{
	if(currentJob != NULL)
		currentJob->setInterrupted(i);
}

/*!
 * This function returns a list of QByteArrays, each of which stores a serialized collection. Note that only
 * collections that have the "isSafedOnExit()" property will be included in this list.
 *
 * Note that you should probably call this once all of our collections will no longer be modified (i.e., in
 * the closeEvent() function on a main window).
 *
 * \return A list of QByteArrays containing the saved collections in our model.
 */
QList<QVariant> CuteSyncCollectionModel::getSerializedList() const
{
	QList<QVariant> r;

	for(int i = 0; i < itemList.count(); ++i)
		if(itemList.at(i)->isSavedOnExit())
			r.append(QVariant(itemList.at(i)->serialize()));

	return r;
}

/*!
 * This function loads a serialized list of collections previously generated by getSerializedList(). Note that any
 * collections we already contain will be cleared.
 *
 * \param c The list of serialized collections to load.
 */
void CuteSyncCollectionModel::loadSerializedList(const QList<QVariant> &c)
{
	for(int i = 0; i < c.count(); ++i)
	{
		qint32 version;
		QString name, path;

		// Create our input stream.

		QDataStream in(c.at(i).value<QByteArray>());
		if(in.status() != QDataStream::Ok) continue;

		// Read our version.

		in >> version;
		if(version > SERIALIZATION_VERSION) continue;

		in.setVersion(version);
		if(in.status() != QDataStream::Ok) continue;

		// Read our name and path.

		in >> name;
		in >> path;

		// Ensure the path actually exists.

		QFileInfo f(path);
		if( (!f.isDir()) || (!f.exists()) )
		{
#ifdef CUTESYNC_DEBUG
std::cout << "Failed to load collection: " << name.toLatin1().data() << "\n";
#endif
			continue;
		}

		// Try to create a new collection object from the given information.

		CuteSyncAbstractCollection *collection = NULL;

		bool u = true;
		for(int j = 0; j < count(); ++j)
		{
			if(collectionAt(j)->getName() == name)
			{
				u = false;
				break;
			}
		}
		if(!u) continue;

		collection = resolver.createCollection(name, path);
		if(collection == NULL)
		{
#ifdef CUTESYNC_DEBUG
std::cout << "Failed to load collection: " << name.toLatin1().data() << "\n";
#endif
			continue;
		}

		// Connect the collection to our slots.

		QObject::connect( collection, SIGNAL( jobStarted(const QString &)     ), this, SLOT( doJobStarted(const QString &)     ) );
		QObject::connect( collection, SIGNAL( progressLimitsUpdated(int, int) ), this, SLOT( doProgressLimitsUpdated(int, int) ) );
		QObject::connect( collection, SIGNAL( progressUpdated(int)            ), this, SLOT( doProgressUpdated(int)            ) );
		QObject::connect( collection, SIGNAL( jobFinished(const QString &)    ), this, SLOT( doJobFinished(const QString &)    ) );

		// Load the collection from the serialized data.
		appendCollection(collection);
		collection->unserialize(c.at(i).value<QByteArray>());
	}
}

/*!
 * This function creates a brand new collection using the given information. Note that we always have ownership of
 * this collection, so even if you get a pointer to it via collectionAt() or something, don't free it. This action
 * will be performed in another thread - connect to this class's signals for status updates.
 *
 * \param n The name for the new collection. This must be unique.
 * \param p The path to the new collection.
 * \param s Whether or not this collection should be saved when we exit.
 */
void CuteSyncCollectionModel::newCollection(const QString &n, const QString &p, bool s)
{
	CuteSyncAbstractCollection *c = NULL;

	// Make sure our name is unique.
	for(int i = 0; i < count(); ++i)
	{
		if(collectionAt(i)->getName() == n)
		{
			QMessageBox::critical(0, tr("Error"),
				tr("Collection name already in use!"));

			return;
		}
	}

	// Create our new collection object.
	c = resolver.createCollection(n, p);

	// If we didn't create one, we must not have been able to determine its type.
	if(c == NULL)
	{
		QMessageBox::critical(0, tr("Error"),
			tr("Unknown/invalid collection type!"));

		return;
	}

	// Set our collectin's save property.
	c->setSaveOnExit(s);

	// Connect the collection to our slots.
	QObject::connect( c, SIGNAL( jobStarted(const QString &)     ), this, SLOT( doJobStarted(const QString &)     ) );
	QObject::connect( c, SIGNAL( progressLimitsUpdated(int, int) ), this, SLOT( doProgressLimitsUpdated(int, int) ) );
	QObject::connect( c, SIGNAL( progressUpdated(int)            ), this, SLOT( doProgressUpdated(int)            ) );
	QObject::connect( c, SIGNAL( jobFinished(const QString &)    ), this, SLOT( doJobFinished(const QString &)    ) );

	// Try to load the collection from the path provided.

	appendCollection(c);
	c->loadCollectionFromPath(p);
}

/*!
 * This function reloads the given collection. This slot is intended to be a thread boundary; the model should be in a different
 * thread than the caller, and using this slot will cross the thread boundary so the reload doesn't tie up the GUI thread.
 *
 * \param c The collection to reload.
 */
void CuteSyncCollectionModel::reloadCollection(CuteSyncAbstractCollection *c)
{ /* SLOT */
	c->reload();
}

/*!
 * This function refreshes the given collection. This slot is intended to be a thread boundary; the model should be in a different
 * thread than the caller, and using this slot will cross the thread boundary so the refresh doesn't tie up the GUI thread.
 *
 * \param c The collection to refresh.
 */
void CuteSyncCollectionModel::refreshCollection(CuteSyncAbstractCollection *c)
{ /* SLOT */
	c->refresh();
}

/*!
 * This function syncronizes the given destination collection with the given source collection. This slot is intended to be a thread
 * bounday; the model should be in a different thread than the caller, and using this slot will cross the thread boundary so the
 * sync doesn't tie up the GUI thread.
 *
 * \param s The source collection.
 * \param d The destination collection.
 */
void CuteSyncCollectionModel::syncCollections(CuteSyncAbstractCollection *s, CuteSyncAbstractCollection *d)
{ /* SLOT */
	d->syncFrom(s);
}

/*!
 * This function handles the visibility of one of our collections changing. We emit a signal alerting any views that
 * are currently displaying us, so they can show/hide the row as needed.
 */
void CuteSyncCollectionModel::doCollectionEnabledChanged()
{ /* SLOT */

	CuteSyncAbstractCollection *c = dynamic_cast<CuteSyncAbstractCollection *>(sender());
	if(c == NULL) return;

	int i = itemList.indexOf(c);
	if(i == -1) return;

	emit rowEnabledChanged(createIndex(i, 0));
}

/*!
 * This slot handles a new job being started by updating our job member, and emitting an appropriate signal.
 *
 * \param j The string describing the job.
 */
void CuteSyncCollectionModel::doJobStarted(const QString &j)
{ /* SLOT */
	CuteSyncAbstractCollection *s = dynamic_cast<CuteSyncAbstractCollection *>(sender());
	currentJob = s;

	if(s != NULL) s->setEnabled(false);

	emit jobStarted(j);
}

/*!
 * This slot handles the progress limits on a job being updated by emitting an appropriate signal.
 *
 * \param min The new minimum limit.
 * \param max The new maximum limit.
 */
void CuteSyncCollectionModel::doProgressLimitsUpdated(int min, int max)
{ /* SLOT */
	emit progressLimitsUpdated(min, max);
}

/*!
 * This slot handles the current progress of a job being updated by emitting an appropriate signal.
 *
 * \param p The new current progress of the current job.
 */
void CuteSyncCollectionModel::doProgressUpdated(int p)
{ /* SLOT */
	emit progressUpdated(p);
}

/*!
 * This slot handles the current job being finished by clearing out our job member, and by emitting an
 * appropriate signal.
 *
 * \param r The return text from the job (e.g., errors or whatever).
 */
void CuteSyncCollectionModel::doJobFinished(const QString &r)
{ /* SLOT */
	CuteSyncAbstractCollection *c = dynamic_cast<CuteSyncAbstractCollection *>(sender());
	if(c != NULL) c->setEnabled(true);

	currentJob = NULL;

	emit jobFinished(r);
}
