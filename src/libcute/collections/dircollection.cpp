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

#include "dircollection.h"

#include <QDirIterator>
#include <QDataStream>
#include <QSet>
#include <QFileInfo>
#include <QList>
#include <QThread>

#include "libcute/defines.h"
#include "libcute/collections/dircollectionconfigwidget.h"
#include "libcute/collections/dirtrack.h"
#include "libcute/tags/filetyperesolver.h"
#include "libcute/tags/taggedfile.h"
#include "libcute/util/guiutils.h"
#include "libcute/util/systemutils.h"
#include "libcute/widgets/collectionmodel.h"

/*!
 * This is our default constructor, which creates a new uninitialized
 * DirCollection with the given QObject as a parent.
 *
 * \param p Our parent object.
 */
CSDirCollection::CSDirCollection(CSCollectionModel *p)
	: CSAbstractCollection(p), recursive(true), organize(true),
		root("")
{
}

/*!
 * This constructor allows our collection's name to be set upon creation, but
 * otherwise leaves it uninitialized.
 *
 * \param n Our collection's name.
 * \param p Our parent object.
 */
CSDirCollection::CSDirCollection(const QString &n,
	CSCollectionModel *p)
	: CSAbstractCollection(n, p), recursive(true), organize(true),
		root("")
{
}

/*!
 * This constructor allows our collection's display descriptor to be set upon
 * creation, but otherwise leaves it uninitialized.
 *
 * \param d Our collection's display descriptor.
 * \param p Our parent object.
 */
CSDirCollection::CSDirCollection(const DisplayDescriptor *d,
	CSCollectionModel *p)
	: CSAbstractCollection(d, p), recursive(true), organize(true),
		root("")
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
CSDirCollection::CSDirCollection(const QString &n,
	const DisplayDescriptor *d, CSCollectionModel *p)
	: CSAbstractCollection(n, d, p), recursive(true), organize(true),
		root("")
{
}

/*!
 * This is our default destructor, which cleans up & destroys our collection.
 */
CSDirCollection::~CSDirCollection()
{
	clear(true);
}

/*!
 * This function returns the icon we would like to be displayed with, based on
 * this particular type of connection.
 *
 * \return Our collection's preferred icon.
 */
QIcon CSDirCollection::getDisplayIcon() const
{
	return CSGUIUtils::getIconFromTheme("folder-sound");
}

/*!
 * This function returns text that might be displayed in an "about collection"
 * window of sorts. Basically, it provides some more detailed information than
 * might be in a normal collection display widget.
 *
 * \return Our about text.
 */
QString CSDirCollection::getAboutText() const
{
	QString r;

	r += QString("Collection Type: Directory\n");
	r += QString("Collection Path: ") + getMountPoint() + QString("\n");
	r += QString("Total Tracks: ") + QString::number(count()) +
		QString("\n");
	r += QString("Used Space: ") + QString::fromStdString(
		CSSystemUtils::getHumanReadableSize(
		CSSystemUtils::getDeviceUsed(getMountPoint()
		.toLatin1().data()))) + QString("\n");
	r += QString("Free Space: ") + QString::fromStdString(
		CSSystemUtils::getHumanReadableSize(
		CSSystemUtils::getDeviceAvailable(getMountPoint()
		.toLatin1().data()))) + QString("\n");
	r += QString("Total Space: ") + QString::fromStdString(
		CSSystemUtils::getHumanReadableSize(
		CSSystemUtils::getDeviceCapacity(
		getMountPoint().toLatin1().data()))) + QString("\n");

	return r;
}

/*!
 * This function loads a new directory-based collection from the disk. By
 * default we flush the current collection (if any), although this behavior is
 * optional.
 *
 * \param p The path to the collection that is to be loaded.
 * \param f Whether the current collection should be flushed or discarded.
 * \return True on success, or false otherwise.
 */
bool CSDirCollection::loadCollectionFromPath(const QString &p, bool f)
{
	setInterruptible(true);
	Q_EMIT jobStarted(tr("Loading collection from path..."));

	// Clear the old collection.
	clear(f);

	// Setup progress bounds.

	int fileCount = static_cast<int>(CSSystemUtils::getFileCount(
		p.toStdString()));
	Q_EMIT progressLimitsUpdated(0, fileCount);

	// Iterate through again to process each file.

	CSDirTrack *track;
	QDirIterator walker(p, QDir::Files | QDir::NoSymLinks,
		getRecursive() ? QDirIterator::Subdirectories :
		QDirIterator::NoIteratorFlags);

	fileCount = 0;
	while(walker.hasNext())
	{
		if(isInterrupted())
		{
			clear(false);
			return false;
		}

		walker.next();

		track = new CSDirTrack(
			walker.fileInfo().absoluteFilePath());

		if(!track->refresh())
		{
			delete track;
			track = NULL;

			Q_EMIT progressUpdated(++fileCount);
			continue;
		}

		addTrack(track);
		Q_EMIT progressUpdated(++fileCount);
	}

	root = walker.path();
	Q_EMIT jobFinished(QString());

	return true;
}

/*!
 * This function refreshes the contents of our directory collection, without
 * reloading tracks that haven't changed (this is determined very simply using
 * the file's path, filesize and last modified time).
 *
 * \return True on success, or false on failure.
 */
bool CSDirCollection::refresh()
{
	setInterruptible(true);
	Q_EMIT jobStarted(tr("Refreshing collection..."));

	// Setup progress bounds.

	int fileCount = static_cast<int>(CSSystemUtils::getFileCount(
		root.toStdString()));
	Q_EMIT progressLimitsUpdated(0, fileCount);

	// Some temporary variables.

	int pcount = 0;
	CSTrack *track;
	QList<int> toRemove;
	QSet<QString> paths;

	// Check if any of our existing tracks need to be updated or removed.

	for(int i = 0; i < count(); ++i)
	{
		if(isInterrupted())
		{
			clear(false);
			return false;
		}

		track = trackAt(i);

		QFileInfo f(track->getPath());

		if(!f.exists())
		{ // If track no longer exists, remove it from the collection.

			toRemove.append(i);

		}
		else if( (! (f.size() == track->getSize()) ) ||
			(f.lastModified() > track->getModifyTime()) )
		{ // Otherwise, if tracks's size or mod. time changed, update.

			track->refresh();
			paths.insert(track->getPath());

		}
		else
		{ // Do nothing with this track.

			paths.insert(track->getPath());

		}

		Q_EMIT progressUpdated(++pcount);
	}

	// Check if there are any new tracks that need to be added.

	QDirIterator walker(root, QDir::Files | QDir::NoSymLinks,
		getRecursive() ? QDirIterator::Subdirectories :
		QDirIterator::NoIteratorFlags);

	while(walker.hasNext())
	{
		if(isInterrupted())
		{
			clear(false);
			return false;
		}

		walker.next();

		// If track isn't already present, create a new one and add it.

		if(!paths.contains(walker.fileInfo().absoluteFilePath()))
		{
			track = new CSDirTrack(
				walker.fileInfo().absoluteFilePath());

			if(!track->refresh())
			{
				delete track;
				track = NULL;

				Q_EMIT progressUpdated(++pcount);
				continue;
			}

			addTrack(track);
		}

		Q_EMIT progressUpdated(++pcount);
	}

	Q_EMIT jobFinished(QString());
	return true;
}

/*!
 * This function returns our collection's mount point - i.e., the parent
 * directory of the collection that was given to loadCollectionFromPath(). If
 * our collection hasn't been initialized, a blank string is returned.
 *
 * \return Our collection's mount point.
 */
QString CSDirCollection::getMountPoint() const
{
	return root;
}

/*!
 * This function returns the path of the track identified by the given key
 * relative to our collection's mount point. If the given key is not in our
 * collection, an empty string is returned.
 *
 * \param k The key to search for.
 * \return The relative path of the specified track.
 */
QString CSDirCollection::getRelativePath(const QString &k) const
{
	return getAbsolutePath(k).replace(getMountPoint(), "");
}

/*!
 * This function returns the absolute path to the track identified by the given
 * key. If the given key is not in our collection, an empty string is returned.
 *
 * \param k The key to search for.
 * \return The absolute path of the specified track.
 */
QString CSDirCollection::getAbsolutePath(const QString &k) const
{
	CSTrack *track = trackAt(k);
	return(track != NULL ? track->getPath() : QString(""));
}

/*!
 * This function returns whether or not our collection is in recursive mode. If
 * it is, then it will include tracks from the mount point an /all
 * subdirectories/, whereas if it isn't only files explicitly in the directory
 * given will be included. We are, by default, in recursive mode.
 *
 * \return True if we are in recursive mode, or false otherwise.
 */
bool CSDirCollection::getRecursive() const
{
	return recursive;
}

/*!
 * This function sets whether or not our collection searches the mount point
 * recursively or not.
 *
 * \param r Whether or not we should look for tracks recursively.
 */
void CSDirCollection::setRecursive(bool r)
{
	recursive = r;
}

/*!
 * This function returns whether or not our collection is set to automatically
 * organize tracks (i.e., file names and directory structure) when our
 * collection is used as a sync destination.
 *
 * \return True if we are automatically organized, or false otherwise.
 */
bool CSDirCollection::isAutoOrganized() const
{
	return organize;
}

/*!
 * This function sets whether or not our collection should automatically
 * organize tracks (i.e., file names and direectory structure) when our
 * collection is used as a sync destination.
 *
 * \param o Whether or not we are automatically organized.
 */
void CSDirCollection::setAutoOrganized(bool o)
{
	organize = o;
}

/*!
 * This function flushes our collection by performing any outstanding I/O
 * actions. Note that, after this function is called, our collection will still
 * be valid, it just will no longer be considered modified.
 *
 * \return True on success, or false on failure.
 */
bool CSDirCollection::flush()
{
	setModified(false);
	return true;
}

/*!
 * This function does some cleanup specific to this type of collection, in
 * addition to calling our superclass's clear() function.
 *
 * \param f Whether or not we should flush before clearing our collection.
 */
void CSDirCollection::clear(bool f)
{
	CSAbstractCollection::clear(f);
	root = "";
}

/*!
 * This function serializes our collection into a QByteArray that is suitable
 * to be stored on a disk to be loaded later. For directory collections, we
 * also serialize all of our track descriptors, because loading track data
 * (tags, audio properties, etc.) is relatively expensive. This means that
 * loading a saved directory collection is MUCH faster than reloading it via
 * loadCollectionFromPath() or similar.
 *
 * \return A QByteArray containing a serialized version of our collection.
 */
QByteArray CSDirCollection::serialize() const
{
	QByteArray obuf;
	QDataStream out(&obuf, QIODevice::ReadWrite);

	out.setVersion(SERIALIZATION_VERSION);
	if(out.status() != QDataStream::Ok) return QByteArray();

	// Write our version number.
	out << static_cast<qint32>(SERIALIZATION_VERSION);

	// Write our collection name.
	out << getName();

	// Write our collection path.
	out << getMountPoint();

	// Write out our collection options.
	out << getRecursive();
	out << isAutoOrganized();

	// Write the number of tracks we contain.

	out << static_cast<qint32>(count());

	// Write out our track descriptors.

	for(int i = 0; i < count(); ++i)
		out << trackAt(i)->serialize();

	return obuf;
}

/*!
 * This function restores a directory collection from the given serialized
 * data. We also call refresh(), so as to ensure that nothing has changed since
 * we were saved - any tracks that have been added, removed or modified will be
 * picked up. Note that this is generally MUCH faster than calling
 * loadCollectionFromPath() or similar.
 *
 * \param d The QByteArray to load our state from.
 */
void CSDirCollection::unserialize(const QByteArray &d)
{
	qint32 version;
	QString n;

	clear(true);

	// Create our input stream.

	QDataStream in(d);
	if(in.status() != QDataStream::Ok) return;

	// Read our version.

	in >> version;
	if(version > SERIALIZATION_VERSION) return;

	in.setVersion(version);
	if(in.status() != QDataStream::Ok) return;

	// Read our name.

	in >> n;
	setName(n);

	// Read + set our path.

	in >> root;

	// Read + set our collection options.

	in >> recursive;
	in >> organize;

	// Read the number of tracks we are expecting.

	qint32 tc;
	in >> tc;

	// Read the track descriptors!

	CSDirTrack *t = NULL;
	QByteArray td;
	for(qint32 i = 0; i < tc; ++i)
	{
#pragma message "TODO - We should provide a parameterless constructor for tracks or make unserialize static"

		t = new CSDirTrack("");
		in >> td;
		t->unserialize(td);

		if(!addTrack(t))
			delete t;
	}

	// Try to refresh our contents now that we've loaded everything.

	setSaveOnExit(true);
	if(!refresh()) clear(false);
}

/*!
 * This function returns a configuration widget specific to this type of
 * collection.
 *
 * \param t The GUI thread our widget should be in.
 * \return A Pointer to the new configuration widget.
 */
CSAbstractCollectionConfigWidget *
	CSDirCollection::getConfigurationWidget() const
{
	CSDirCollectionConfigWidget *w =
		new CSDirCollectionConfigWidget();

	QObject::connect(w, SIGNAL(applyRequest()),
		this, SLOT(doConfigurationApply()));
	QObject::connect(w, SIGNAL(resetRequest()),
		this, SLOT(doConfigurationReset()));

	return w;
}

/*!
 * This function deletes the track identified by the given key from our
 * collection.
 *
 * \param k The key of the track to be deleted.
 * \return True on success, or false on failure.
 */
bool CSDirCollection::quietDeleteTrack(const QString &k)
{
	CSDirTrack *track = dynamic_cast<CSDirTrack *>(trackAt(k));
	if(track == NULL) return false;

	if(!QFile::remove(track->getPath())) return false;

	removeTrack(k);
	return true;
}

/*!
 * This function copies the specified track from the given other collection to
 * our collection.
 *
 * \param s The collection we are copying from.
 * \param k The key identifying the track we want in the source collection.
 * \return True on success, or false on failure.
 */
bool CSDirCollection::quietCopyTrack(const CSAbstractCollection *s,
	const QString &k)
{
	// Do some sanity checks.

	if(s == this)
		return true;

	if(!s->containsKey(k))
		return false;

	// Figure out where we are going to put the file.

	QString dPath = getAbsoluteWritePath(s, k);
	QString dDir = dPath.left( dPath.lastIndexOf(QDir::separator()) );

	// Try creating the path.

	QDir d;
	if(!d.mkpath(dDir))
		return false;

	// Define our read buffer and the input+output files.

	char buf[5242880]; // 5MiB buffer.
	QFile i(s->getAbsolutePath(k));
	QFile o(dPath);

	// Try opening our files.

	if(!i.open(QIODevice::ReadOnly))
		return false;

	if(!o.open(QIODevice::WriteOnly))
	{
		i.close();
		return false;
	}

	// Do the copy!

	int64_t read;
	while( (read = i.read(buf, 5242880)) > 0 )
		o.write(buf, read);

	i.close();
	o.close();

	// Add the new track to our collection.

	CSDirTrack *track = new CSDirTrack(dPath);
	if(!track->refresh())
	{
		delete track;
		QFile::remove(dPath);
		return false;
	}

	addTrack(track);

#pragma message "Notify that we have potentially messed up sorting"

	return true;
}

/*!
 * This function processes a given string, and returns a version of it which is
 * valid for filenames (i.e., with all non-ASCII characters removed or
 * replaced).
 *
 * \param s The original string to process.
 * \return The new, filename-friendly string.
 */
QString CSDirCollection::filenameProcess(const QString &s) const
{
	// Remove or replace nonstandard characters.

	QByteArray b = s.toLatin1();

	b.replace('&', "And");
	b.replace('+', "Plus");
	b.replace((char) 0x91, "ae");
	b.replace((char) 0x92, "AE");

	for(int i = 0; i < b.count(); ++i)
	{
		switch(static_cast<unsigned char>(b[i]))
		{
			case 0x2F: b[i] = '-'; break;
			case 0x5B: b[i] = '('; break;
			case 0x5C: b[i] = '-'; break;
			case 0x5D: b[i] = ')'; break;
			case 0x7B: b[i] = '('; break;
			case 0x7D: b[i] = ')'; break;
			case 0x7E: b[i] = '-'; break;
			case 0x80: b[i] = 'C'; break;
			case 0x81: b[i] = 'u'; break;
			case 0x82: b[i] = 'e'; break;
			case 0x83: b[i] = 'a'; break;
			case 0x84: b[i] = 'a'; break;
			case 0x85: b[i] = 'a'; break;
			case 0x86: b[i] = 'a'; break;
			case 0x87: b[i] = 'c'; break;
			case 0x88: b[i] = 'e'; break;
			case 0x89: b[i] = 'e'; break;
			case 0x8A: b[i] = 'e'; break;
			case 0x8B: b[i] = 'i'; break;
			case 0x8C: b[i] = 'i'; break;
			case 0x8D: b[i] = 'i'; break;
			case 0x8E: b[i] = 'A'; break;
			case 0x8F: b[i] = 'A'; break;
			case 0x90: b[i] = 'E'; break;
			case 0x93: b[i] = 'o'; break;
			case 0x94: b[i] = 'o'; break;
			case 0x95: b[i] = 'o'; break;
			case 0x96: b[i] = 'u'; break;
			case 0x97: b[i] = 'u'; break;
			case 0x98: b[i] = 'y'; break;
			case 0x99: b[i] = 'O'; break;
			case 0x9A: b[i] = 'U'; break;
			case 0xA0: b[i] = 'a'; break;
			case 0xA1: b[i] = 'i'; break;
			case 0xA2: b[i] = 'o'; break;
			case 0xA3: b[i] = 'u'; break;
			case 0xA4: b[i] = 'n'; break;
			case 0xA5: b[i] = 'N'; break;

			case 0x01: case 0x02: case 0x03: case 0x04:
			case 0x05: case 0x06: case 0x07: case 0x08:
			case 0x09: case 0x0A: case 0x0B: case 0x0C:
			case 0x0D: case 0x0E: case 0x0F: case 0x10:
			case 0x11: case 0x12: case 0x13: case 0x14:
			case 0x15: case 0x16: case 0x17: case 0x18:
			case 0x19: case 0x1A: case 0x1B: case 0x1C:
			case 0x1D: case 0x1E: case 0x1F: case 0x21:
			case 0x22: case 0x23: case 0x24: case 0x25:
			case 0x2A: case 0x2C: case 0x2E: case 0x3A: case 0x3B:
			case 0x3C: case 0x3D: case 0x3E: case 0x3F:
			case 0x40: case 0x5E: case 0x60: case 0x7C:
			case 0x7F: case 0x9B: case 0x9C: case 0x9D:
			case 0x9E: case 0x9F: case 0xA6: case 0xA7:
			case 0xA8: case 0xA9: case 0xAA: case 0xAB:
			case 0xAC: case 0xAD: case 0xAE: case 0xAF:
			case 0xB0: case 0xB1: case 0xB2: case 0xB3:
			case 0xB4: case 0xB5: case 0xB6: case 0xB7:
			case 0xB8: case 0xB9: case 0xBA: case 0xBB:
			case 0xBC: case 0xBD: case 0xBE: case 0xBF:
			case 0xC0: case 0xC1: case 0xC2: case 0xC3:
			case 0xC4: case 0xC5: case 0xC6: case 0xC7:
			case 0xC8: case 0xC9: case 0xCA: case 0xCB:
			case 0xCC: case 0xCD: case 0xCE: case 0xCF:
			case 0xD0: case 0xD1: case 0xD2: case 0xD3:
			case 0xD4: case 0xD5: case 0xD6: case 0xD7:
			case 0xD8: case 0xD9: case 0xDA: case 0xDB:
			case 0xDC: case 0xDD: case 0xDE: case 0xDF:
			case 0xE0: case 0xE1: case 0xE2: case 0xE3:
			case 0xE4: case 0xE5: case 0xE6: case 0xE7:
			case 0xE8: case 0xE9: case 0xEA: case 0xEB:
			case 0xEC: case 0xED: case 0xEE: case 0xEF:
			case 0xF0: case 0xF1: case 0xF2: case 0xF3:
			case 0xF4: case 0xF5: case 0xF6: case 0xF7:
			case 0xF8: case 0xF9: case 0xFA: case 0xFB:
			case 0xFC: case 0xFD: case 0xFE: case 0xFF:
				b[i] = 0x00;
				break;
		};
	}

	// Remove null characters, and add a new trailing null.

	b.replace((char) 0x00, "");
	b.append('\0');

	// Return our string!

	return QString::fromLatin1(b.constData());
}

/*!
 * This function determines the absolute path to which the given file should be
 * written (i.e., when the given track is copied from the given source
 * collection into our collection). This is either the same relative path as it
 * had in the source collection if isAutoOrganized() is false, or a new path
 * based upon the track's tags otherwise.
 *
 * The following processing is universal (regardless of which method is used):
 *     - We ensure filenames contain only universally valid characters (so,
 *       ASCII, basically).
 *     - We ensure that filenames are unique; as long as the track is unique,
 *       we prevent file collisions.
 *
 * Also note that any path returned by this function has had QDir::cleanPath()
 * called on it (this removes superfluous directory separators and resolves
 * any "."'s and ".."'s found in the path).
 *
 * \param s The source collection.
 * \param k The key identifying the track we want in the source collection.
 * \return The absolute path to which the given track should be written.
 */
QString CSDirCollection::getAbsoluteWritePath(
	const CSAbstractCollection *s, const QString &k) const
{
	QString r;

	// Sanity checks.

	if(!s->containsKey(k))
		return "";

	// Generate the path.

	if(isAutoOrganized())
	{
		CSFileTypeResolver resolv;
		CSTaggedFile t(s->getAbsolutePath(k), resolv, false);

		// Formulate the relative path - "/Artist/Album/TN Title.ext".

		r = QString("%1").arg(t.getTrackNumber());
		if(r.length() == 1) r.prepend('0');

		r.prepend(
			filenameProcess(t.getArtist()) + QDir::separator() +
			filenameProcess(t.getAlbum()) + QDir::separator()
		);

		r.append(" " + filenameProcess(t.getTitle()) + "." +
			t.getFileExtension());
	}
	else
	{
		// Use the same relative path as in the source collection.

		r = s->getRelativePath(k);
	}

	// Ensure we are using consistent separators.

	r.replace('/', QDir::separator());
	r.replace('\\', QDir::separator());

	// Make the new path absolute.

	r.prepend(getMountPoint() + QDir::separator());
	r = QDir::cleanPath(r);

	/*
	 * Ensure there is no file collision (assume the track is unique -
	 * collisions are purely a naming issue).
	 */

	if(QFile::exists(r))
	{
		QString dup;
		int i = 1;

		do
		{
			dup = r;
			dup.insert(dup.lastIndexOf('.'),
				QString("_%1").arg(i++));
		} while(QFile::exists(dup));

		r = dup;
	}

	// Return the path.

	return r;
}

/*!
 * This slot handles one of our configuration widgets requesting that its
 * current state be applied to our collection.
 */
void CSDirCollection::doConfigurationApply()
{ /* SLOT */

	CSDirCollectionConfigWidget *w =
		dynamic_cast<CSDirCollectionConfigWidget *>(sender());

	if(w != NULL)
	{
		setSaveOnExit(w->getSaveState());
		setAutoOrganized(w->getOrganizeState());
	}

}

/*!
 * This slot handles our configuration widget requesting a reset by resetting
 * its options to our currently stored values.
 */
void CSDirCollection::doConfigurationReset()
{ /* SLOT */

	CSDirCollectionConfigWidget *w =
		dynamic_cast<CSDirCollectionConfigWidget *>(sender());

	if(w != NULL)
	{
		w->setSaveState(isSavedOnExit());
		w->setOrganizeState(isAutoOrganized());
	}

}
