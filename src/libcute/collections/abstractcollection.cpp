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

#include "abstractcollection.h"

#include <QDataStream>

#include "libcute/defines.h"
#include "libcute/collections/abstractcollectionconfigwidget.h"
#include "libcute/collections/generalcollectionconfigwidget.h"
#include "libcute/collections/track.h"
#include "libcute/widgets/collectionmodel.h"

/*!
 * This is our default constructor, which creates a new uninitialized collection
 * with the given QObject as a parent.
 *
 * \param p Our parent object.
 */
CSAbstractCollection::CSAbstractCollection(
	CSCollectionModel *p)
	: QAbstractTableModel(p), name(""), modified(false), enabled(true),
		interrupted(false), saveOnExit(false), displayDescriptor(NULL)
{
}

/*!
 * This constructor allows our collection's name to be set upon creation, but
 * otherwise leaves it uninitialized.
 *
 * \param n Our collection's name.
 * \param p Our parent object.
 */
CSAbstractCollection::CSAbstractCollection(const QString &n,
	CSCollectionModel *p)
	: QAbstractTableModel(p), name(n), modified(false), enabled(true),
		interrupted(false), saveOnExit(false), displayDescriptor(NULL)
{
}

/*!
 * This constructor allows our collection's display descriptor to be set upon
 * creation, but otherwise leaves it uninitialized.
 *
 * \param d Our collection's display descriptor.
 * \param p Our parent object.
 */
CSAbstractCollection::CSAbstractCollection(
	const DisplayDescriptor *d, CSCollectionModel *p)
	: QAbstractTableModel(p), name(""), modified(false), enabled(true),
		interrupted(false), saveOnExit(false), displayDescriptor(d)
{
}

/*!
 * This constructor allows our collection's name and display descriptor to be
 * set upon creation, but otherwise leaves it uninitialized.
 *
 * \param n Our collection's name.
 * \param d Our collection's display descriptor.
 * \param p Our parent object.
 */
CSAbstractCollection::CSAbstractCollection(const QString &n,
	const DisplayDescriptor *d, CSCollectionModel *p)
	: QAbstractTableModel(p), name(n), modified(false), enabled(true),
		interrupted(false), saveOnExit(false), displayDescriptor(d)
{
}

/*!
 * This is a placeholder destructor, so we can properly subclass
 * QAbstractTableModel.
 */
CSAbstractCollection::~CSAbstractCollection()
{
}

/*!
 * This function returns our collection configuration widget. This widget
 * controls the settings our collection has (e.g., whether or not it is saved
 * on exit). This particular function will return only options that are common
 * to all collection types.
 *
 * If your subclass has its own special options, then you should override this
 * method which returns a widget containing both the widget returned by the
 * superclass, as well as your custom configuration widget.
 *
 * Note that we create this widget without a 'parent' QObject, due to the fact
 * that the collection is necessarily in a different thread than the GUI. This
 * means that IT IS UP TO THE CALLER TO ENSURE THAT THE WIDGET RETURNED IS
 * DELETED.
 *
 * \param t The thread our config widget should be a member of (the GUI thread).
 * \return Our collection's configuration widget.
 */
CSAbstractCollectionConfigWidget *
	CSAbstractCollection::getConfigurationWidget() const
{
	CSGeneralCollectionConfigWidget *w =
		new CSGeneralCollectionConfigWidget();

	QObject::connect(w, SIGNAL(applyRequest()),
		this, SLOT(doConfigurationApply()));
	QObject::connect(w, SIGNAL(resetRequest()),
		this, SLOT(doConfigurationReset()));

	return w;
}

/*!
 * This function returns the data for the element at the given row and column
 * in a format that should be displayed directly. The data returned by this
 * function probably shouldn't be compared.
 *
 * \param r The desired row.
 * \param c The desired column.
 * \return The data at the given position, or Invalid if row/column was invalid.
 */
QVariant CSAbstractCollection::getDisplayData(int r, int c) const
{
	if(displayDescriptor == NULL)
		return QVariant(QVariant::Invalid);

	if(!displayDescriptor->columns.contains(c))
		return QVariant(QVariant::Invalid);

	CSAbstractCollection::Column col =
		displayDescriptor->columns.value(c);
	QVariant d = getSortData(r, col);

	switch(col)
	{
		// Convert the length to an hh:mm:ss format.
		case CSAbstractCollection::Length:
			d = QVariant(CSTrack::getLengthDisplay(
				d.value<int>()));
			break;

		default:
			break;
	};

	return d;
}

/*!
 * This function returns the sorting data for the element at the given row and
 * column. This is not to be modified from the data stored by each track --
 * it should be in a format that is comparable.
 *
 * \param r The desired row.
 * \param c The desired column.
 * \return The data at the given position, or Invalid if row/column was invalid.
 */
QVariant CSAbstractCollection::getSortData(int r,
	CSAbstractCollection::Column c) const
{
	if(displayDescriptor == NULL) return QVariant(QVariant::Invalid);
	if( (r < 0) || (r >= count()) ) return QVariant(QVariant::Invalid);

	return trackSort.at(r)->getColumn(c);
}

/*!
 * This function clears our current collection, making our object equivalent to
 * a default-constructed one. Whether or not we flush() first is optional.
 *
 * Please note that IF YOU OVERRIDE THIS FUNCTION to perform your own cleanup,
 * you STILL NEED TO CALL THIS FUNCTION, or your subclass will leak memory.
 *
 * \param f Whether or not we should flush before clearing our collection.
 */
void CSAbstractCollection::clear(bool f)
{
	if(f) flush();

	trackHash.clear();
	while(!trackSort.isEmpty())
		delete trackSort.takeLast();

	modified = false;
}

/*!
 * This function returns our collection's current name. See setName() for more
 * details.
 *
 * \return Our collection's name.
 */
QString CSAbstractCollection::getName() const
{
	return name;
}

/*!
 * This function sets the name of our collection. This should be a short
 * (10-ish characters would be pretty reasonable) identifier that can be used
 * in a GUI to represent our collection. We don't do any validity checking on
 * this value here; it is up to our parent object to make sure it is
 * usable/unique/etc.
 *
 * \param n Our collection's new name.
 */
void CSAbstractCollection::setName(const QString &n)
{
	name = n;
}

/*!
 * This function tests whether or not this collection is currently enabled.
 * Collections should be disabled when some action is being taken on them that
 * could be messed up if something else happens at the same time. For instance,
 * synchronizing means that the collection shouldn't be displayed until it's
 * done, and you probably shouldn't try to copy tracks or delete tracks at the
 * same time either.
 *
 * \return True if we are enabled, or false otherwise.
 */
bool CSAbstractCollection::isEnabled() const
{
	return enabled;
}

/*!
 * This function sets whether or not our collection is enabled. You should
 * probably call this in subclasses in functions that are starting some big
 * action, e.g., synchronizing.
 *
 * \param e Our new enabled state (true means enabled, false means disabled).
 */
void CSAbstractCollection::setEnabled(bool e)
{
	enabled = e;
	Q_EMIT enabledChanged();
}

/*!
 * This function tests whether or not our collection expects to be saved on
 * program exit. Saving a collection means serializing it and storing it on the
 * disk, so the next time the program is launched it can be refreshed far more
 * quickly than the initial collection loading.
 *
 * \return True if we should be saved on exit, or false otherwise.
 */
bool CSAbstractCollection::isSavedOnExit() const
{
	return saveOnExit;
}

/*!
 * This function sorts our collection according to our display descriptor. If
 * no display descriptor has been set, then no action is taken.
 */
void CSAbstractCollection::sort() const
{
	if(displayDescriptor == NULL) return;
	quicksort(0, count() - 1);
}

/*!
 * This function discards any changes that have been made to our current
 * collection and simply reloads it.
 *
 * \return True on success, or false on failure.
 */
bool CSAbstractCollection::reload()
{
	bool r = loadCollectionFromPath( getMountPoint(), false );
	return r;
}

/*!
 * This function refreshes the contents of our collection. By default, it is
 * equivalent to calling reload(), but a subclass could override it to, for
 * instance, avoid reloading tracks that haven't changed.
 *
 * \return True on success, or false on failure.
 */
bool CSAbstractCollection::refresh()
{
	return reload();
}

/*!
 * This function sets whether or not our collection should be saved on exit.
 *
 * \param s True if we should save on exit, or false otherwise.
 */
void CSAbstractCollection::setSaveOnExit(bool s)
{ /* SLOT */

	saveOnExit = s;

}

/*!
 * This function deletes a series of tracks from our collection, if present. If
 * you give us track keys that are not present in the collection, then we just
 * ignore them. Errors are reported through our jobFinished() signal, as well
 * as through our return value.
 *
 * \param k The list of track keys that should be deleted.
 * \return True if no errors occured, or false otherwise.
 */
bool CSAbstractCollection::deleteTracks(const QStringList &k)
{ /* SLOT */
	QString r, t;

	Q_EMIT jobStarted(tr("Deleting tracks..."), false);
	Q_EMIT progressLimitsUpdated(0, k.count());

	for(int p = 0; p < k.count(); ++p)
	{
		if(interrupted)
		{
			Q_EMIT jobFinished(QString());
			return false;
		}

		if(!quietDeleteTrack(k.at(p)))
		{
			r.append(QString("Failed to delete: %1\n")
				.arg(k.at(p)));
		}

		Q_EMIT progressUpdated(p+1);
	}

	Q_EMIT jobFinished(r);
	return r.isEmpty();
}

/*!
 * This function copies a series of tracks from a given other collection to our
 * collection. If you give us track keys that are not present in the soruce
 * collection, then we just ignore them. Errors are reported through our
 * jobFinished() signal, as well as through our return value.
 *
 * \param s The source collection to get the tracks from.
 * \param k A list of track keys that should be copied.
 * \return True if no errors occurred, or false otherwise.
 */
bool CSAbstractCollection::copyTracks(
	const CSAbstractCollection *s, const QStringList &k)
{ /* SLOT */
	QString r, t;

	Q_EMIT jobStarted(tr("Copying tracks..."), false);
	Q_EMIT progressLimitsUpdated(0, k.count());

	for(int p = 0; p < k.count(); ++p)
	{
		if(interrupted)
		{
			Q_EMIT jobFinished(QString());
			return false;
		}

		if(!quietCopyTrack(s, k.at(p)))
			r.append(QString("Failed to copy: %1\n").arg(k.at(p)));

		Q_EMIT progressUpdated(p+1);
	}

	Q_EMIT jobFinished(r);
	return r.isEmpty();
}

/*!
 * This is a convenience function that syncs our collection to the given other
 * collection. That is, after we are done, our two collections will contain the
 * same tracks. Note that we DO NOT touch the source collection. Errors are
 * reported through our jobFinished() signal, as well as through our return
 * value.
 *
 * \param o The source collection to sync from.
 * \return True on success, or false on failure.
 */
bool CSAbstractCollection::syncFrom(CSAbstractCollection *o)
{ /* SLOT */
	QString r, t;

	Q_EMIT jobStarted(tr("Synchronizing collections..."), false);
	o->setEnabled(false);

	QList<QString> del = keysDifference(o), cp = o->keysDifference(this);

	Q_EMIT progressLimitsUpdated(0, del.count() + cp.count());
	int p = 0;

	// Delete stuff first.
	while(!del.isEmpty())
	{
		if(interrupted)
		{
			flush();
			o->setEnabled(true);
			Q_EMIT jobFinished(QString());
			return false;
		}

		t = del.takeLast();
		if(!quietDeleteTrack(t))
			r.append(QString("Failed to delete: %1\n").arg(t));

		Q_EMIT progressUpdated(++p);
	}

	// Now copy new stuff.
	while(!cp.isEmpty())
	{
		if(interrupted)
		{
			flush();
			o->setEnabled(true);
			Q_EMIT jobFinished(QString());
			return false;
		}

		t = cp.takeLast();
		if(!quietCopyTrack(o, t))
			r.append(QString("Failed to copy: %1\n").arg(t));

		Q_EMIT progressUpdated(++p);
	}

	flush();

	o->setEnabled(true);
	Q_EMIT jobFinished(r);
	return r.isEmpty();
}

/*!
 * This function tests whether or not our collection contains a track with the
 * given key.
 *
 * \param k The key to search for.
 * \return True if the key is found, or false otherwise.
 */
bool CSAbstractCollection::containsKey(const QString &k) const
{
	return trackHash.contains(k);
}

/*!
 * This function returns the number of tracks in our collection.
 *
 * \return The number of tracks in the collection.
 */
int CSAbstractCollection::count() const
{
	return trackHash.count();
}

/*!
 * This is a convenience function that merely tests if our collection is empty
 * or not.
 *
 * \return True if our collection is empty, or false otherwise.
 */
bool CSAbstractCollection::isEmpty() const
{
	return trackHash.isEmpty();
}

/*!
 * This function returns a QList containing all of the keys present in our
 * collection.
 *
 * \return A list of our keys.
 */
QList<QString> CSAbstractCollection::getKeysList() const
{
	return trackHash.keys();
}

/*!
 * This function returns a QList containing all of the keys that ARE present in
 * our collection, but that are NOT present in the given other collection. This
 * can be useful e.g. for syncing two collections.
 *
 * \param o The other collection to compare ourself to.
 * \return A list of keys in our collection that are not in the other one.
 */
QList<QString> CSAbstractCollection::keysDifference(
	const CSAbstractCollection *o) const
{
#pragma message "TODO - This functions performance should be improved"

	QList<QString> srcK = getKeysList(), destK = o->getKeysList();
	QList<QString> diff;

	for(int i = 0; i < srcK.count(); ++i)
		if(!destK.contains(srcK.at(i)))
			diff.append(srcK.at(i));

	return diff;
}

/*!
 * This function returns whether or not our collection has been modified since
 * we last called flush().
 *
 * \return True if our collection has been modified, or false otherwise.
 */
bool CSAbstractCollection::isModified() const
{
	return modified;
}

/*!
 * This function retrieves our collection's display descriptor, or returns NULL
 * if none has been set as of yet.
 *
 * \return Our display descriptor.
 */
const CSAbstractCollection::DisplayDescriptor *
	CSAbstractCollection::getDisplayDescriptor() const
{
	return displayDescriptor;
}

/*!
 * This function sets our collection's current display descriptor. Note that if
 * one hasn's been set (either via a constructor or this function) then it is
 * assumed that our object is not going to be displayed by a QTableView. If you
 * try to do so anyway, we will just be returning invalid QVariants for all
 * requested data; create a display descriptor first.
 *
 * Also note that we never take ownership of the display descriptor; it is
 * ALWAYS going to be something created by our displaying object and given to
 * us, and as such it is up to our display object to free it if need be.
 *
 * Note that this function automagically sorts the data in our model, updates
 * our column count, and then emits the appropriate signals to alert any views
 * using us.
 *
 * \param d The display descriptor to use.
 */
void CSAbstractCollection::setDisplayDescriptor(
	const DisplayDescriptor *d)
{
	Q_EMIT beginResetModel();

	displayDescriptor = d;
	sort();

	Q_EMIT endResetModel();
}

/*!
 * This is one of our QAbstractTableModel functions. It returns the number of
 * rows in our collection. This is equivalent to count().
 *
 * \param p Our parent model index. This is ignored.
 * \return The number of rows in our collection.
 */
int CSAbstractCollection::rowCount(const QModelIndex &UNUSED(p)) const
{
	return count();
}

/*!
 * This is one of our QAbstractTableModel functions. It returns the number of
 * columns in our collection.
 *
 * \param p Our parent model index. This is ignored.
 * \return The number of columns in our collection.
 */
int CSAbstractCollection::columnCount(const QModelIndex &UNUSED(p)) const
{
	if(displayDescriptor != NULL)
		return displayDescriptor->columns.count();
	else
		return 0;
}

/*!
 * This is one of our QAbstractTableModel functions. It returns (as a QVariant)
 * the data that should be displayed in a specific cell in our table.
 *
 * \param i The index of the cell whose data we are retrieving.
 * \param r The cell's display role.
 * \return The data to be displayed.
 */
QVariant CSAbstractCollection::data(const QModelIndex &i, int r) const
{
	if(displayDescriptor == NULL)
		return QVariant(QVariant::Invalid);

	switch(r)
	{
		case Qt::DisplayRole:
			return getDisplayData(i.row(), i.column());
			break;

		case Qt::ToolTipRole:
			return QVariant(QVariant::Invalid);
			break;

		case Qt::AccessibleDescriptionRole:
			return headerData(i.column(),
				Qt::Horizontal, Qt::DisplayRole);
			break;

		default:
			return QVariant(QVariant::Invalid);
			break;
	};
}

/*!
 * This is one of our QAbstractTableModel functions. It returns (as a QVariant)
 * the data that should be displayed in a specific spot in our table header.
 *
 * \param s The section (i.e., column) desired.
 * \param o The orientation (either horizontal or vertical).
 * \param r The header cell's display role.
 * \return The data to be displayed.
 */
QVariant CSAbstractCollection::headerData(
	int s, Qt::Orientation o, int r) const
{
	if(o != Qt::Horizontal)
		return QVariant(QVariant::Invalid);

	if(r != Qt::DisplayRole)
		return QVariant(QVariant::Invalid);

	if(!displayDescriptor->columns.contains(s))
	{
		return QVariant(QVariant::Invalid);
	}
	else
	{
		return QVariant(CSAbstractCollection::getColumnName(
			displayDescriptor->columns.value(s)));
	}
}

/*!
 * This function sets whether or not our current job has been interrupted. If
 * you interrupt a job, then the job is expected to notice and halt itself
 * after some reasonably short amount of time. Note that this will work
 * identically regardless if the job is interruptible or not - if you want to
 * be really nice about it, you are expected to check isInterruptible()
 * yourself.
 *
 * \param i True means interrupt, false means don't.
 */
void CSAbstractCollection::setInterrupted(bool i)
{
	interrupted = i;
}

/*!
 * This function sets our current job as being "interrupted". This is
 * equivalent to calling setInterrupted(true), except that, since it is a slot,
 * it can be safely called from other threads.
 */
void CSAbstractCollection::setInterrupted()
{ /* SLOT */

	setInterrupted(true);

}

/*!
 * This is a utility function for our subclasses which returns a list of all of
 * our tracks, in an arbitrary order. This is useful if the subclass, for
 * instance, needs to perform some action to ALL tracks, so using trackAt()
 * would be inefficient.
 *
 * \return A list of all of the tracks in our collection.
 */
QList<CSTrack *> CSAbstractCollection::allTracks() const
{
	return trackHash.values();
}

/*!
 * This function returns a pointer to the track descriptor stored at the given
 * row in our collection. This function returns NULL if the given index is
 * out-of-bounds.
 *
 * \param r The row of the desired track.
 * \return The desired track descriptor, or NULL.
 */
CSTrack *CSAbstractCollection::trackAt(int r) const
{
	if( (r < 0) || (r >= count()) ) return NULL;
	return trackSort.at(r);
}

/*!
 * This function returns a pointer to the track descriptor stored with the
 * given key in our collection. This function returns NULL if the given key is
 * not in our collection.
 *
 * \param k They key of the desired track.
 * \return The desired track descriptor, or NULL.
 */
CSTrack *CSAbstractCollection::trackAt(const QString &k) const
{
	return trackHash.value(k, NULL);
}

/*!
 * This function removes the track descriptor at the given row. If the given
 * index is out-of-bounds, then no action is taken. Note that we have ownership
 * of all track pointers - so the memory will be freed appropriately. Note,
 * though, that we won't delete anything from the disk.
 *
 * \param r The row of the desired track.
 */
void CSAbstractCollection::removeTrack(int r)
{
	if( (r < 0) || (r >= count()) ) return;

	CSTrack *track = trackSort.takeAt(r);
	trackHash.remove(track->getHash());
	delete track;
}

/*!
 * This function removes the track descriptor with the given hash. If the given
 * hash is not present in our collection, then no action is taken. Note that we
 * have ownership of all track pointers - so the memory will be freed
 * appropriately. Note, though, that we won't delete anything from the disk.
 *
 * \param k The key of the desired track.
 */
void CSAbstractCollection::removeTrack(const QString &k)
{
	CSTrack *track = trackHash.value(k, NULL);
	if(track == NULL) return;
	trackSort.removeAll(track);
	delete track;
}

/*!
 * This function appends the given track descriptor to our collection. Note
 * that you may want to manually call sort() after doing this. Note that if our
 * collection already contains a track with the same hash, then no action is
 * taken. You should be updating the existing track, if necessary.
 *
 * Also note that if the track is successfully added (i.e., we return true),
 * then we take ownership of that track object - you SHOULD NOT free it
 * yourself.
 *
 * \param t The track descriptor to add.
 * \return True if the track was added, or false otherwise.
 */
bool CSAbstractCollection::addTrack(CSTrack *t)
{
	if(t == NULL) return false;
	if(trackHash.contains(t->getHash())) return false;

	trackHash.insert(t->getHash(), t);
	trackSort.append(t);
	return true;
}

/*!
 * This function tests if the current job has been asked to interrupt itself.
 * See setInterrupted() for more information. Subclasses are expected to check
 * the return value of this function periodically while performing a job, so as
 * to exit as nicely as possible when asked. Note that you should terminate
 * REGARDLESS of whether or not your job is considered "interruptible."
 *
 * \return True if we have been interrupted, or false otherwise.
 */
bool CSAbstractCollection::isInterrupted() const
{
	return interrupted;
}

/*!
 * This function sets whether our collection has been modified since the last
 * time flush() was called. This function should be called by our subclasses so
 * our flush() functionality will work correctly.
 *
 * \param m True if the collection has been modified, or false otherwise.
 */
void CSAbstractCollection::setModified(bool m)
{
	modified = m;
}

/*!
 * This is our backend quicksort function that sorts our collection's items. It
 * accepts a left and right index, so it can be used recursively. It decides
 * /how/ to do the sorting based on our display descriptor. WE EXPECT OUR
 * CALLER TO MAKE SURE OUR OBJECT HAS A VALID DISPLAY DESCRIPTOR IF YOU DON'T
 * IT WILL SEGFAULT!
 *
 * \param l The left-most index to sort.
 * \param r The right-most index to sort.
 */
void CSAbstractCollection::quicksort(int l, int r) const
{
	int pivot, i, j;

	// Choose a pivot value.
	pivot = l + ((r - l) / 2);

	// Initialize loop indices.
	i = l;
	j = r;

	// Get a compare modifier; if descending, we need to flip our compares.

	int cm = displayDescriptor->s_order == Qt::AscendingOrder ? 1 : -1;

	/*
	 * Keep looping until we have swapped all values to the correct side of
	 * the pivot.
	 */

	while(i <= j)
	{
		/*
		 * Move right until we find a value on the wrong side of the
		 * pivot.
		 */

		while((sortCmp(i, pivot) * cm) == -1)
			++i;

		/*
		 * Move left until we find a value that is on the wrong side of
		 * the pivot.
		 */

		while((sortCmp(j, pivot) * cm) == 1)
			--j;

		/*
		 * If our left index is still left of our right index, then
		 * swap the two items.
		 */

		if(i < j)
		{
			// Update our pivot if we are swapping its location.
			if(i == pivot)
				pivot = j;
			else if(j == pivot)
				pivot = i;

			// Swap our elements.
			trackSort.swap(i, j);
		}

		// Move indices one more place for the next iteration.

		if(i <= j)
		{
			++i;
			--j;
		}
	}

	// If our left-most chunk is of size >= 1, then recurse on it.
	if(l < j) quicksort(l, j);

	// If our right-most chunk is of size >= 1, then recurse on it.
	if(i < r) quicksort(i, r);
}

/*!
 * This function compares two track descriptors. It returns -1 if a < b, 0 if
 * a = b, and 1 if a > b. WE EXPECT OUR CALLER TO MAKE SURE OUR OBJECT HAS A
 * VALID DISPLAY DESCRIPTOR IF YOU DON'T IT WILL SEGFAULT!
 *
 * \param ra The first row to compare.
 * \param rb The second row to compare.
 * \return The result of the comparison.
 */
int CSAbstractCollection::sortCmp(int ra, int rb) const
{
	Column c;

	// A row is obviously equal to itself.s

	if(ra == rb)
		return 0;

	for(int i = 0; i < displayDescriptor->s_columns.count(); ++i)
	{
		c = displayDescriptor->s_columns.at(i);
		QVariant ad = getSortData(ra, c), bd = getSortData(rb, c);

		switch(c)
		{
			// QString type columns.
			case Artist:
			case Album:
			case Title:
			{
				if(ad.value<QString>() < bd.value<QString>())
				{
					return -1;
				}
				else if(ad.value<QString>() >
					bd.value<QString>())
				{
					return 1;
				}
			}
			break;

			// Integer type columns.
			case DiscNumber:
			case TrackNumber:
			case TrackCount:
			case Length:
			case Year:
			{
				if(ad.value<int>() < bd.value<int>())
					return -1;
				else if(ad.value<int>() > bd.value<int>())
					return 1;
			}
			break;
		};
	}

	return 0;
}

/*!
 * This slot handles one of our configuration widgets requesting that its
 * current state be applied to our collection.
 */
void CSAbstractCollection::doConfigurationApply()
{ /* SLOT */

	CSGeneralCollectionConfigWidget *w =
		dynamic_cast<CSGeneralCollectionConfigWidget *>(sender());

	if(w != NULL)
	{
		setSaveOnExit(w->getSaveState());
	}

}

/*!
 * This slot handles our configuration widget requesting a reset by resetting
 * its options to our currently stored values.
 */
void CSAbstractCollection::doConfigurationReset()
{ /* SLOT */

	CSGeneralCollectionConfigWidget *w =
		dynamic_cast<CSGeneralCollectionConfigWidget *>(sender());

	if(w != NULL)
	{
		w->setSaveState(isSavedOnExit());
	}

}

/*!
 * This function returns a default display descriptor for use with collections.
 * This descriptor uses sane defaults, so all columns are enabled in a
 * reasonable order, and sorting goes by Artist, then Album, then DiscNumber,
 * then TrackNumber (which is, probably, what most people expect anyway).
 *
 * \return A default display descriptor.
 */
CSAbstractCollection::DisplayDescriptor
	CSAbstractCollection::getDefaultDisplayDescriptor()
{
	DisplayDescriptor d;

	d.s_order = Qt::AscendingOrder;

	d.s_columns.clear();
	d.s_columns.append(CSAbstractCollection::Artist);
	d.s_columns.append(CSAbstractCollection::Album);
	d.s_columns.append(CSAbstractCollection::DiscNumber);
	d.s_columns.append(CSAbstractCollection::TrackNumber);

	d.columns.clear();
	d.columns.insert(0, CSAbstractCollection::Artist);
	d.columns.insert(1, CSAbstractCollection::Album);
	d.columns.insert(2, CSAbstractCollection::Title);
	d.columns.insert(3, CSAbstractCollection::DiscNumber);
	d.columns.insert(4, CSAbstractCollection::TrackNumber);
	d.columns.insert(5, CSAbstractCollection::Length);
	d.columns.insert(6, CSAbstractCollection::Year);

	return d;
}

/*!
 * This function takes a display descriptor and serializes it into a
 * QByteArray, which could (for instance) be saved to a file on the disk so as
 * to be loaded later on. The original object can be restored using
 * unserializeDisplayDescriptor().
 *
 * \param d The display descriptor to serialize.
 * \return A serialized byte array representing the given descriptor.
 */
QByteArray CSAbstractCollection::serializeDisplayDescriptor(
	const CSAbstractCollection::DisplayDescriptor &d)
{
	QByteArray obuf;
	QDataStream out(&obuf, QIODevice::ReadWrite);

	out.setVersion(SERIALIZATION_VERSION);
	if(out.status() != QDataStream::Ok) return QByteArray();

	// Write our version number.
	out << static_cast<qint32>(SERIALIZATION_VERSION);

	// Write our display descriptor.

	qint32 order = d.s_order;

	QList<QVariant> scols;
	for(int i = 0; i < d.s_columns.count(); ++i)
		scols.append(QVariant(d.s_columns.at(i)));

	QList<int> dcolskeys = d.columns.keys();
	QHash<QString, QVariant> dcols;
	for(int i = 0; i < dcolskeys.count(); ++i)
	{
		dcols.insert(QString::number(dcolskeys.at(i)),
			QVariant(d.columns.value(dcolskeys.at(i))));
	}

	out << order;
	out << scols;
	out << dcols;

	return obuf;
}

/*!
 * This function attempts to restore a display descriptor object from the given
 * serialized QByteArray.
 *
 * \param d The byte array to read a display descriptor from.
 * \return A new display descriptor object containing the desired data.
 */
CSAbstractCollection::DisplayDescriptor
	CSAbstractCollection::unserializeDisplayDescriptor(
	const QByteArray &d)
{
	DisplayDescriptor dd;
	qint32 version;

	// Create our input stream.

	QDataStream in(d);
	if(in.status() != QDataStream::Ok)
		return getDefaultDisplayDescriptor();

	// Read our version.

	in >> version;
	if(version > SERIALIZATION_VERSION)
		return getDefaultDisplayDescriptor();

	in.setVersion(version);
	if(in.status() != QDataStream::Ok)
		return getDefaultDisplayDescriptor();

	// Try to read our display descriptor.

	qint32 order;
	QList<QVariant> scols;
	QHash<QString, QVariant> dcols;

	in >> order;
	in >> scols;
	in >> dcols;

	// Translate this input into our normal format.

	dd.s_order = static_cast<Qt::SortOrder>(order);

	dd.s_columns.clear();
	for(int i = 0; i < scols.count(); ++i)
	{
		dd.s_columns.append(
			static_cast<CSAbstractCollection::Column>(
			scols.at(i).value<qint32>()));
	}

	dd.columns.clear();
	QList<QString> dcolskeys = dcols.keys();
	for(int i = 0; i < dcolskeys.count(); ++i)
	{
		dd.columns.insert(dcolskeys.at(i).toInt(),
			static_cast<CSAbstractCollection::Column>(
				dcols.value(dcolskeys.at(i)).value<qint32>()));
	}

	return dd;
}

/*!
 * This function returns the "pretty name" for a column. These strings are
 * typically a bit longer than what is returned by getColumnName(), but they
 * are probably better to use in settings where space is not limited.
 *
 * \param c The column whose name is desired.
 * \return The "pretty name" for the given column.
 */
QString CSAbstractCollection::getColumnPrettyName(Column c)
{
	switch(c)
	{
		case Artist:
			return "Artist Name";

		case Album:
			return "Album Title";

		case Title:
			return "Track Title";

		case DiscNumber:
			return "CD Number";

		case TrackNumber:
			return "Track Number";

		case TrackCount:
			return "Track Count";

		case Length:
			return "Song Length";

		case Year:
			return "Release Year";
	};

	return QString();
}

/*!
 * This function returns the string name of the provided column. These are
 * typically shorter than those returned by getColumnPrettyName(), and are
 * appropriate in settings where the length of the string to be displayed
 * should be as small as possible.
 *
 * \param c The desired column
 * \return The column's string name.
 */
QString CSAbstractCollection::getColumnName(Column c)
{
	switch(c)
	{
		case Artist:
			return "Artist";

		case Album:
			return "Album";

		case Title:
			return "Title";

		case DiscNumber:
			return "CD #";

		case TrackNumber:
			return "Track #";

		case TrackCount:
			return "Tracks";

		case Length:
			return "Length";

		case Year:
			return "Year";
	};

	return QString();
}
