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

#include "collectionmodel.h"

#ifdef CUTESYNC_DEBUG
	#include <iostream>
#endif

#include <QIcon>
#include <QDataStream>
#include <QFileInfo>
#include <QMessageBox>

#include "libcute/defines.h"
#include "libcute/collections/abstractcollection.h"
#include "libcute/thread/collectionthreadpool.h"
#include "libcute/widgets/collectionlistitem.h"

/*!
 * This is our default constructor, which creates a new model object with the
 * given parent.
 *
 * \param p Our parent object.
 */
CSCollectionModel::CSCollectionModel(QObject *p)
	: QAbstractListModel(p)
{
	threadPool = new CSCollectionThreadPool(this);

	// Connect the thread pool's progress signals to our signals.

	QObject::connect(threadPool, SIGNAL(jobStarted(const QString &, bool)),
		this, SIGNAL(jobStarted(const QString &, bool)));
	QObject::connect(threadPool, SIGNAL(progressLimitsUpdated(int, int)),
		this, SIGNAL(progressLimitsUpdated(int, int)));
	QObject::connect(threadPool, SIGNAL(progressUpdated(int)),
		this, SIGNAL(progressUpdated(int)));
	QObject::connect(threadPool, SIGNAL(jobFinished(const QString &)),
		this, SIGNAL(jobFinished(const QString &)));

	// Connect our actions to the thread pool's slots.

	QObject::connect(this, SIGNAL(startNew(const QString &,
		const QString &, bool)), threadPool, SLOT(newCollection(
		const QString &, const QString &, bool)));

	// Connect the thread pool's result signals to our slots / signals.

	QObject::connect(threadPool, SIGNAL(collectionCreated(
		CSAbstractCollection *)), this, SLOT(doCollectionCreated(
		CSAbstractCollection *)));
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSCollectionModel::~CSCollectionModel()
{
	clear();
}

/*!
 * This function returns the number of rows our model contains. This is another
 * way of calling count().
 *
 * \return The number of rows in our model.
 */
int CSCollectionModel::rowCount(const QModelIndex &UNUSED(p)) const
{
	return count();
}

/*!
 * This function returns the display data for a given cell in the model. This
 * function handles both the display text of a collection (its name) and the
 * icon it uses (via getDisplayIcon() on the collection superclass).
 *
 * \param i The index in our model. The column index is ignored.
 * \param r The display role for the desired data.
 */
QVariant CSCollectionModel::data(const QModelIndex &i, int r) const
{
	switch(r)
	{
		case Qt::DisplayRole:
			{
				CSAbstractCollection *c =
					collectionAt(i.row());

				if(c != NULL)
					return QVariant(c->getName());
				else
					return QVariant(QVariant::Invalid);
			}

		case Qt::DecorationRole:
			{
				CSAbstractCollection *c =
					collectionAt(i.row());

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
 * This function returns the desired header data. Because our model doesn't
 * have column/row headers by design, we just return an invalid QVariant.
 *
 * \param s The section (row/col) for the data.
 * \param o The orientation of the data.
 * \param r The display role of the data.
 */
QVariant CSCollectionModel::headerData(int UNUSED(s),
	Qt::Orientation UNUSED(o), int UNUSED(r)) const
{
	return QVariant(QVariant::Invalid);
}

/*!
 * This function clears our items list, deleting all items present.
 */
void CSCollectionModel::clear()
{
	CSCollectionListItem *i;

	while(!itemList.isEmpty())
	{
		i = itemList.takeFirst();
		delete i;
	}
}

/*!
 * This function returns the number of items currently in our model. This value
 * is also used by rowCount() to provide the same information to the view using
 * us as a model.
 *
 * \return The number of items in our list.
 */
int CSCollectionModel::count() const
{
	return itemList.count();
}

/*!
 * This function returns the collection pointer that is in our model at the
 * given position (i.e., row). If the position provided is out-of-bounds, NULL
 * is returned instead.
 *
 * \param i The index (row) of the desired collection.
 * \return The desired collection.
 */
CSAbstractCollection *CSCollectionModel::collectionAt(int i) const
{
	if( (i < 0) || (i >= count()) )
		return NULL;

	return itemList.at(i)->getCollection();
}

/*!
 * This function returns a pointer to the collection in our model with the
 * given name. Note that the string comparison we do is case-sensitive.
 *
 * \param n The name to search for.
 * \return A pointer to the desired collection, or NULL if it is not found.
 */
CSAbstractCollection *CSCollectionModel::collectionFromName(
	const QString &n) const
{
	CSAbstractCollection *c;

	for(int i = 0; i < count(); ++i)
	{
		c = collectionAt(i);

		if(c->getName() == n)
			return c;
	}

	return NULL;
}

/*!
 * This function removes the collection at the given index in our model. Note
 * that this deletes the collection, since we take ownership of anything added
 * to us; be sure not to try and use a different pointer to the same collection
 * after you call this.
 *
 * Note that it is probably unnecessary to check if the collection is currently
 * working, since active collections are not clickable in order to be removed.
 * It is, however, up to the caller to perform this check if it is indeed
 * needed for whatever reason.
 *
 * \param i The index (row) of the collection to be removed.
 */
void CSCollectionModel::removeCollectionAt(int i)
{
	CSAbstractCollection *c = collectionAt(i);

	if(c == NULL)
		return;

	CSCollectionListItem *it = itemForCollection(c);

	if(it == NULL)
		return;

	Q_EMIT beginResetModel();
#pragma message "TODO - Lock the collection from starting any new jobs."
	bool inter = c->isInterruptible();

	if(!inter)
	{
#pragma message "TODO - Prompt the user to ask if they want to remove anyway."
	}

	itemList.removeAt(itemList.indexOf(it));

	c->setInterrupted(true);

	delete it;
	c->deleteLater();

	Q_EMIT endResetModel();
}

/*!
 * This function appends the given collection to our model. Note that from here
 * on our WE ARE TAKING OWNERSHIP OF THE COLLECTION - this means that we will
 * delete it if need be. Note that this is probably the only function you
 * should be calling to add new items to the model, unless you have a really
 * good reason for calling something else.
 *
 * \param c The collection to add to our model.
 */
void CSCollectionModel::appendCollection(CSAbstractCollection *c)
{
	if(c == NULL)
		return;

	for(int i = 0; i < count(); ++i)
		if(collectionAt(i) == c)
			return;

	CSCollectionListItem *i = new CSCollectionListItem(c, this);

	QObject::connect(c, SIGNAL(enabledChanged()),
		this, SLOT(doCollectionEnabledChanged()));

	itemList.append(i);

	Q_EMIT dataChanged(createIndex(itemList.count() - 1, 0),
		createIndex(itemList.count() - 1, 0));
}

/*!
 * This function returns a list of the names of all of the collections in our
 * model. This could be used, e.g., to populate a combo box or something.
 *
 * \return A list of the names in our model.
 */
QList<QString> CSCollectionModel::getCollectionNameList() const
{
	QList<QString> r;

	for(int i = 0; i < count(); ++i)
		r.append(collectionAt(i)->getName());

	return r;
}

/*!
 * This function returns a list of QByteArrays, each of which stores a
 * serialized collection. Note that only collections that have the
 * "isSafedOnExit()" property will be included in this list.
 *
 * Note that you should probably call this once all of our collections will no
 * longer be modified (i.e., in the closeEvent() function on a main window).
 *
 * \return A list of QByteArrays containing the saved collections in our model.
 */
QList<QVariant> CSCollectionModel::getSerializedList() const
{
	QList<QVariant> r;
	CSAbstractCollection *c;

	for(int i = 0; i < count(); ++i)
	{
		c = collectionAt(i);

		if(c->isSavedOnExit())
			r.append(QVariant(c->serialize()));
	}

	return r;
}

/*!
 * This function attempts to our worker thread(s) gracefully by telling our
 * thread pool to stop gracefully.
 */
void CSCollectionModel::stopGracefully()
{
	threadPool->stopGracefully();
}

/*!
 * This function loads a serialized list of collections previously generated by
 * getSerializedList(). Note that any collections we already contain will be
 * cleared.
 *
 * \param c The list of serialized collections to load.
 */
void CSCollectionModel::loadSerializedList(const QList<QVariant> &c)
{

	/*
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

		// Try creating new collection object from the given info.

		CSAbstractCollection *collection = NULL;

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

		QObject::connect(collection,
			SIGNAL(jobStarted(const QString &)), this,
			SLOT(doJobStarted(const QString &)));
		QObject::connect(collection,
			SIGNAL(progressLimitsUpdated(int, int)), this,
			SLOT(doProgressLimitsUpdated(int, int)));
		QObject::connect(collection, SIGNAL(progressUpdated(int)),
			this, SLOT(doProgressUpdated(int)));
		QObject::connect(collection,
			SIGNAL(jobFinished(const QString &)), this,
			SLOT(doJobFinished(const QString &)));

		// Load the collection from the serialized data.
		appendCollection(collection);
		collection->unserialize(c.at(i).value<QByteArray>());
	}
	*/

}

/*!
 * This function creates a brand new collection using the given information.
 * Note that we always have ownership of this collection, so even if you get a
 * pointer to it via collectionAt() or something, don't free it. This action
 * will be performed in another thread - connect to this class's signals for
 * status updates.
 *
 * \param n The name for the new collection. This must be unique.
 * \param p The path to the new collection.
 * \param s Whether or not this collection should be saved when we exit.
 */
void CSCollectionModel::newCollection(const QString &n,
	const QString &p, bool s)
{

	// Make sure our name is unique.

	for(int i = 0; i < count(); ++i)
	{
		if(collectionAt(i)->getName() == n)
		{
#pragma message "TODO - Improve warning generation?"
			QMessageBox::critical(0, tr("Error"),
				tr("Collection name already in use!"));

			return;
		}
	}

	Q_EMIT startNew(n, p, s);

}

/*!
 * This function reloads the given collection. This slot is intended to be a
 * thread boundary; the model should be in a different thread than the caller,
 * and using this slot will cross the thread boundary so the reload doesn't tie
 * up the GUI thread.
 *
 * \param c The collection to reload.
 */
void CSCollectionModel::reloadCollection(CSAbstractCollection *c)
{ /* SLOT */

	c->reload();

}

/*!
 * This function refreshes the given collection. This slot is intended to be a
 * thread boundary; the model should be in a different thread than the caller,
 * and using this slot will cross the thread boundary so the refresh doesn't
 * tie up the GUI thread.
 *
 * \param c The collection to refresh.
 */
void CSCollectionModel::refreshCollection(CSAbstractCollection *c)
{ /* SLOT */

	c->refresh();

}

/*!
 * This function syncronizes the given destination collection with the given
 * source collection. This slot is intended to be a thread bounday; the model
 * should be in a different thread than the caller, and using this slot will
 * cross the thread boundary so the sync doesn't tie up the GUI thread.
 *
 * \param s The source collection.
 * \param d The destination collection.
 */
void CSCollectionModel::syncCollections(CSAbstractCollection *s,
	CSAbstractCollection *d)
{ /* SLOT */

	d->syncFrom(s);

}

/*!
 * This function returns the collection list item which represents the given
 * collection in our model. If no such list item could be found, then we return
 * NULL instead.
 *
 * \param c The collection to find an item for.
 * \return A pointer to the list item representing the given collection.
 */
CSCollectionListItem *CSCollectionModel::itemForCollection(
	const CSAbstractCollection *c)
{
	for(int i = 0; i < itemList.count(); ++i)
		if(itemList.at(i)->getCollection() == c)
			return itemList.at(i);

	return NULL;
}

/*!
 * This function handles the visibility of one of our collections changing. We
 * emit a signal alerting any views that are currently displaying us, so they
 * can show/hide the row as needed.
 */
void CSCollectionModel::doCollectionEnabledChanged()
{ /* SLOT */

	CSAbstractCollection *c =
		dynamic_cast<CSAbstractCollection *>(sender());

	if(c == NULL)
		return;

	int idx = -1;

	for(int i = 0; i < count(); ++i)
	{
		if(collectionAt(i) == c)
		{
			idx = i;
			break;
		}
	}

	if(idx == -1)
		return;

	Q_EMIT rowEnabledChanged(createIndex(idx, 0));

}

/*!
 * This slot handles a new collection being created by appending that
 * collection to our model.
 *
 * \param c The newly-created collection.
 */
void CSCollectionModel::doCollectionCreated(CSAbstractCollection *c)
{ /* SLOT */

	appendCollection(c);

}
