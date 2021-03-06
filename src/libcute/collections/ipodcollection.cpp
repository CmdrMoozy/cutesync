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

#include "ipodcollection.h"

#include <cstdint>
#include <cstdlib>

#ifdef CUTESYNC_DEBUG
	#include <iostream>
#endif

#include <QFile>
#include <QList>
#include <QDir>
#include <QDataStream>
#include <QFileInfo>

#include "libcute/defines.h"
#include "libcute/collections/ipodcollectionconfigwidget.h"
#include "libcute/collections/ipodtrack.h"
#include "libcute/tags/filetyperesolver.h"
#include "libcute/tags/taggedfile.h"
#include "libcute/util/guiutils.h"
#include "libcute/util/systemutils.h"
#include "libcute/widgets/collectionmodel.h"

extern "C" {
	#include <gdk-pixbuf/gdk-pixbuf.h>
}

#ifdef CUTESYNC_DEBUG
	/*!
	 * This is a debugging function which creates a new false iPod
	 * directory structure in the given path. This is mainly useful for
	 * testing purposes.
	 *
	 * \param n The name of the new iPod.
	 * \param p The path in which to initialize a new iPod structure.
	 * \return True on success, or false otherwise.
	 */
	bool CSIPodCollection::createFalseIPod(
		const QString &n, const QString &p)
	{
		GError *error = NULL;

		if(!itdb_init_ipod(p.toStdString().c_str(), "C297",
			n.toStdString().c_str(), &error))
		{
			if(error != NULL)
				g_error_free(error);

			return false;
		}

		return true;
	}
#endif

/*!
 * This is our default constructor, which creates a new uninitialized
 * IPodCollection with the given QObject as a parent.
 *
 * \param p Our parent object.
 */
CSIPodCollection::CSIPodCollection(CSCollectionModel *p)
	: CSAbstractCollection(p), optionsModified(false), artwork(true),
		caselessSort(true), ignorePrefixes(true), itdb(NULL), root("")
{
}

/*!
 * This constructor allows our collection's name to be set upon creation, but
 * otherwise leaves it uninitialized.
 *
 * \param n Our collection's name.
 * \param p Our parent object.
 */
CSIPodCollection::CSIPodCollection(const QString &n,
	CSCollectionModel *p)
	: CSAbstractCollection(n, p), optionsModified(false),
		artwork(true), caselessSort(true), ignorePrefixes(true),
		itdb(NULL), root("")
{
}

/*!
 * This constructor allows our collection's display descriptor to be set upon
 * creation, but otherwise leaves it uninitialized.
 *
 * \param d Our collection's display descriptor.
 * \param p Our parent object.
 */
CSIPodCollection::CSIPodCollection(const DisplayDescriptor *d,
	CSCollectionModel *p)
	: CSAbstractCollection(d, p), optionsModified(false),
		artwork(true), caselessSort(true), ignorePrefixes(true),
		itdb(NULL), root("")
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
CSIPodCollection::CSIPodCollection(const QString &n,
	const DisplayDescriptor *d, CSCollectionModel *p)
	: CSAbstractCollection(n, d, p), optionsModified(false),
		artwork(true), caselessSort(true), ignorePrefixes(true),
		itdb(NULL), root("")
{
}

/*!
 * This is our default destructor. It flushes the current iTunes DB to the disk
 * (if any), and then frees any memory we still have allocated.
 */
CSIPodCollection::~CSIPodCollection()
{
	clear(true);
}

/*!
 * This function returns the icon we would like to be displayed with, based on
 * this particular type of connection.
 *
 * \return Our collection's preferred icon.
 */
QIcon CSIPodCollection::getDisplayIcon() const
{
	return CSGUIUtils::getIconFromTheme("multimedia-player-apple-ipod");
}

/*!
 * This function returns text that might be displayed in an "about collection"
 * window of sorts. Basically, it provides some more detailed information than
 * might be in a normal collection display widget.
 *
 * \return Our about text.
 */
QString CSIPodCollection::getAboutText() const
{
	QString r;

	r += QString("Collection Type: iPod\n");
	r += QString("Collection Path: ") + getMountPoint() + QString("\n");
	r += QString("Total Tracks: ") + QString::number(count()) + QString("\n");
	r += QString("Used Space: ") + QString::fromStdString(
		CSSystemUtils::getHumanReadableSize(
		CSSystemUtils::getDeviceUsed(
		getMountPoint().toLatin1().data()))) + QString("\n");
	r += QString("Free Space: ") + QString::fromStdString(
		CSSystemUtils::getHumanReadableSize(
		CSSystemUtils::getDeviceAvailable(
		getMountPoint().toLatin1().data()))) + QString("\n");
	r += QString("Total Space: ") + QString::fromStdString(
		CSSystemUtils::getHumanReadableSize(
		CSSystemUtils::getDeviceCapacity(
		getMountPoint().toLatin1().data()))) + QString("\n");

	if(itdb != NULL)
	{
		const Itdb_IpodInfo *itdbInfo =
			itdb_device_get_ipod_info(itdb->device);

		r += QString("\n");
		r += QString("iPod Model Number: ") + QString::fromUtf8(
			itdbInfo->model_number) + QString("\n");
		r += QString("iPod Generation: ") + QString::fromUtf8(
			itdb_info_get_ipod_generation_string(
			itdbInfo->ipod_generation)) + QString("\n");
		r += QString("iPod Reported Capacity: ") + QString::number(
			itdbInfo->capacity, 'f', 2) + QString(" GiB\n");
		r += QString("\n");
		r += QString("Supports artwork? ") + QString(
			itdb_device_supports_artwork(itdb->device) ?
			"Yes!" : "No") + QString("\n");
		r += QString("Supports chapter image? ") + QString(
			itdb_device_supports_chapter_image(itdb->device) ?
			"Yes!" : "No") + QString("\n");
		r += QString("Supports photos? ") + QString(
			itdb_device_supports_photo(itdb->device) ?
			"Yes!" : "No") + QString("\n");
		r += QString("Supports podcsts? ") + QString(
			itdb_device_supports_podcast(itdb->device) ?
			"Yes!" : "No") + QString("\n");
		r += QString("Supports videos? ") + QString(
			itdb_device_supports_video(itdb->device) ?
			"Yes!" : "No") + QString("\n");
	}

	return r;
}

/*!
 * This function loads a new iTunes DB from the given iPod mount point. By
 * default we flush the current collection (if any), although this behavior is
 * optional.
 *
 * \param p The path at which an iPod is mounted.
 * \param f Whether or not we flush the current collection or just discard it.
 * \return True on success, or false on failure.
 */
bool CSIPodCollection::loadCollectionFromPath(const QString &p, bool f)
{
#pragma message "TODO - Use slotsignal error reporting"

	GList *trackList = NULL;
	GError *error = NULL;
	Itdb_iTunesDB *i = NULL;
	int pr = 0;

	Q_EMIT jobStarted(tr("Loading collection from path..."), true);

	// Clean up the current collection.

	clear(f);

	// Try loading the given collection

	i = itdb_parse(p.toUtf8().data(), &error);

	if( (error != NULL) || (i == NULL) )
	{
		if(error != NULL)
		{
#ifdef CUTESYNC_DEBUG
std::cout << "Error while loading collection from path: " <<
	error->message << "\n";
#endif

			g_error_free(error);
			error = NULL;
		}
		else
		{
#ifdef CUTESYNC_DEBUG
std::cout << "Unknown error while loading collection from path.\n";
#endif
		}

		if(i != NULL) itdb_free(i);
		return false;
	}

	itdb = i;

	// Iterate through the collection, and grab the tracks we care about.

	Q_EMIT progressLimitsUpdated(0, itdb_tracks_number(itdb));
	trackList = g_list_first(itdb->tracks);
	while(trackList != NULL)
	{
		if(isInterrupted())
		{
			clear(false);
			return false;
		}

		Itdb_Track *t = static_cast<Itdb_Track *>(trackList->data);
		trackList = trackList->next;
		Q_EMIT progressUpdated(++pr);

		if(t->mediatype == 0x00000001)
		{
			// 0x 00 00 00 01 means AUDIO; all we care about.
			addTrack(new CSIPodTrack(t));
		}
	}

	// Set our class members.

	setModified(false);

	QDir mp(p);
	root = mp.absolutePath();

	// Set our collection options.

	refreshCollectionOptions();

	// Return success!

	Q_EMIT jobFinished(QString());
	return true;
}

/*!
 * This function returns our collection's mount point - i.e., the path that was
 * passed to loadCollectionFromPath(). If a collection hasn't been successfully
 * loaded yet, a blank string is returned instead.
 *
 * \return Our collection's mount point.
 */
QString CSIPodCollection::getMountPoint() const
{
	return root;
}

/*!
 * This function returns the path of the track denoted by the given key
 * relative to our colleciton's mount point. If the given key is not in our
 * collection, a blank string is returned instead.
 *
 * \param k The key to search for.
 * \return The track's path relative to our mount point.
 */
QString CSIPodCollection::getRelativePath(const QString &k) const
{
	CSIPodTrack *t = dynamic_cast<CSIPodTrack *>(trackAt(k));
	if(t == NULL) return QString("");

	gchar *p = g_strdup(t->getTrack()->ipod_path);
	itdb_filename_ipod2fs(p);

	QString r = QString::fromUtf8(p);
	g_free(p);
	return r;
}

/*!
 * This function returns the absolute path of the track denoted by the given
 * key. This is equivalent, more or less, to simply appending the track's
 * relative path to our collection's mount point. If the given key is not in
 * our collection, a blank string is returned instead.
 *
 * \param k The key to search for.
 * \return The track's absolute path.
 */
QString CSIPodCollection::getAbsolutePath(const QString &k) const
{
	return containsKey(k) ? (getMountPoint() +
		getRelativePath(k)) : QString("");
}

/*!
 * This function returns whether or not album artwork syncing is enabled. When
 * enabled, we will automatically try to find appropriate album artwork for
 * tracks being added to this collection, and add them to the iTunes DB
 * appropriately.
 *
 * \return True if album artwork is enabled, or false otherwise.
 */
bool CSIPodCollection::getAlbumArtworkEnabled() const
{
	return artwork;
}

/*!
 * This function sets whether or not album artwork syncing is enabled. When
 * enabled, we will automatically try to find appropriate album artwork for
 * tracks being added to this collection, and add them to the iTunes DB
 * appropriately.
 *
 * \param a True if album artwork should be enabled, or false otherwise.
 */
void CSIPodCollection::setAlbumArtworkEnabled(bool a)
{
	artwork = a;
}

/*!
 * This function returns whether or not we configure the iPod to sort things
 * case-insensitively when we are used as a sync destination.
 *
 * \return True if sorting should be case-insensitive, or false otherwise.
 */
bool CSIPodCollection::getCaseInsensitiveSort() const
{
	return caselessSort;
}

/*!
 * This function sets whether or not we configure the iPod to sort things
 * case-insensitively when we are used as a sync destination.
 *
 * \param c True if sorting should be case-insensitive, or false otherwise.
 */
void CSIPodCollection::setCaseInsensitiveSort(bool c)
{
	optionsModified |= (c != caselessSort);
	caselessSort = c;
}

/*!
 * This function returns whether or not we configure the iPod to ignore common
 * title prefixes for sorting purposes (e.g., "The ...") when we are used as a
 * sync destination.
 *
 * \return True if we should ignore common title prefixes, or false otherwise.
 */
bool CSIPodCollection::getIgnoreCommonPrefixes() const
{
	return ignorePrefixes;
}

/*!
 * This function sets whether or not we configure the iPod to ignore common
 * title prefixes for sorting purposes (e.g., "The ...") when we are used as a
 * sync destination.
 *
 * \param i True if we should ignore common title prefixes, or false otherwise.
 */
void CSIPodCollection::setIgnoreCommonPrefixes(bool i)
{
	optionsModified |= (i != ignorePrefixes);
	ignorePrefixes = i;
}

/*!
 * This function flushes the iTunes DB we are currently working on to the disk.
 * That is, it will be written to the device, and we will mark ourself as no
 * longer having been modified.
 *
 * \return True on success, or false on failure.
 */
bool CSIPodCollection::flush()
{
	if(itdb == NULL) return false;

	/* If our options have changed, apply them to the iTunes DB before it
	 * is flushed to the device.
	 */

	if(optionsModified)
	{
		// Write our new options to the options file.

		QFile optionsFile(getMountPoint() +
			"/iPod_Control/iTunes/CuteSync.prefs");

		if(optionsFile.open(QIODevice::WriteOnly |
			QIODevice::Truncate))
		{
			char options[16];
			for(int i = 0; i < 16; ++i)
				options[i] = 0;

			if(caselessSort)
				options[0] |= 0x80;

			if(ignorePrefixes)
				options[0] |= 0x40;

			optionsFile.write(options, 16);
		}

		// Update all of our tracks' sorting information.

		QList<CSTrack *> tracks = allTracks();
		while(!tracks.empty())
		{
			CSIPodTrack *t =
				dynamic_cast<CSIPodTrack *>(
				tracks.takeFirst());

			if(t != NULL)
			{
				t->applySortOptions(
					caselessSort, ignorePrefixes);
			}
		}

		/*
		 * The new options have been applied successfully, but our
		 * tracks have (potentially) been modified.
		 */

		optionsModified = false;
		setModified(true);
	}

	// Write the iTunes DB to the device.

	GError *error = NULL;
	if(isModified())
	{
		if(!itdb_write(itdb, &error))
		{
			if(error != NULL)
			{
#ifdef CUTESYNC_DEBUG
std::cout << "Error writing iTunes DB: " << error->message << "\n";
#endif

				g_error_free(error);
				error = NULL;
				return false;
			}
			else
			{
#ifdef CUTESYNC_DEBUG
std::cout << "Unknown error writing iTunes DB.\n";
#endif

				return false;
			}
		}
	}

	setModified(false);
	return true;
}

/*!
 * This function does some cleanup specific to this type of collection, in
 * addition to calling our superclass's clear() function.
 *
 * \param f Whether or not we should flush before clearing our collection.
 */
void CSIPodCollection::clear(bool f)
{
	CSAbstractCollection::clear(f);

	if(itdb != NULL)
	{
		itdb_free(itdb);
		itdb = NULL;
	}

	root = "";
}

/*!
 * This function serializes our collection into a QByteArray so it could be,
 * e.g., saved to the disk to be loaded later. Note that, because of the way
 * iPods work, we don't bother to serialize track descriptors - it is already
 * really fast to load them from the device, so it isn't worth it.
 *
 * \return A QByteArray containing our collection's state.
 */
QByteArray CSIPodCollection::serialize() const
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

	// Write our collection options.

	out << artwork;

	return obuf;
}

/*!
 * This function restores our collection from a serialized QByteArray. This
 * mostly just restores the collection name and path; tracks are re-loaded from
 * the iTunes database on the device.
 *
 * \param d The QByteArray to restore our state from.
 */
void CSIPodCollection::unserialize(const QByteArray &d)
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

	// Read our collection options.

	in >> artwork;

	// Refresh the collection.

	setSaveOnExit(true);
	if(!refresh()) clear(false);

	// Set our collection options from the parsed collection.

	refreshCollectionOptions();
}

/*!
 * This function returns a configuration widget specific to this type of
 * collection.
 *
 * \param t The GUI thread our widget should be in.
 * \return A Pointer to the new configuration widget.
 */
CSAbstractCollectionConfigWidget *
	CSIPodCollection::getConfigurationWidget() const
{
	CSIPodCollectionConfigWidget *w =
		new CSIPodCollectionConfigWidget();

	QObject::connect(w, SIGNAL(applyRequest()),
		this, SLOT(doConfigurationApply()));
	QObject::connect(w, SIGNAL(resetRequest()),
		this, SLOT(doConfigurationReset()));

	return w;
}

/*!
 * This function deletes a track from our collection by performing the
 * following steps:
 *
 *     - Deleting the actual file from the device.
 *     - Removing the file from any playlists it's a member of.
 *         - Removing any playlists that are now empty as a result.
 *     - Removing the track from the iTunes DB.
 *
 * This function doesn't write the iTunes DB to the disk directly; you need to
 * call flush() to do that.
 *
 * \param k The key of the track that we are about to remove.
 * \return True on success, or false on failure.
 */
bool CSIPodCollection::quietDeleteTrack(const QString &k)
{
	if(itdb == NULL) return false;
	CSIPodTrack *track = dynamic_cast<CSIPodTrack *>(
		trackAt(k));
	if(track == NULL) return false;

	// Delete the file itself from the iPod's disk.

	gchar *path = itdb_filename_on_ipod(track->getTrack());

	if(path == NULL) return false;
	if( !QFile::remove( QString::fromUtf8(path) ) ) return false;

	g_free(path);

	// Remove the track from all playlists it is a member of.

	QList<Itdb_Playlist *> emptyPlaylists;
	GList *plList = g_list_first(itdb->playlists);
	while(plList != NULL)
	{
		Itdb_Playlist *pl = static_cast<Itdb_Playlist *>(plList->data);
		plList = plList->next;

		// Remove the track from this playlist, if it is present.
		itdb_playlist_remove_track(pl, track->getTrack());

		// If the playlist is now empty, set it to be removed later.
		if(itdb_playlist_tracks_number(pl) == 0)
			emptyPlaylists.append(pl);
	}

	/*
	 * Go back and remove any playlists that are now empty (as long as they
	 * aren't the MPL!).
	 */

	for(int i = 0; i < emptyPlaylists.count(); ++i)
	{
		if(!itdb_playlist_is_mpl(emptyPlaylists.at(i)))
			itdb_playlist_remove(emptyPlaylists.at(i));
	}

	// Remove the track itself from our collection and the iTunes DB.

	QString hash = track->getHash();
	itdb_track_remove(track->getTrack());
	removeTrack(hash);

	// Set our database as having been modified.

	setModified(true);
	return true;
}

/*!
 * This function copies a single track from the given source collection, and
 * with the given key, to this collection.
 *
 * \param s The source collection we are copying from.
 * \param k The key of the track we are copying.
 * \return True on success, or false on failure.
 */
bool CSIPodCollection::quietCopyTrack(
	const CSAbstractCollection *s, const QString &k)
{
#pragma message "TODO - Use slotsignal error reporting"

	if(itdb == NULL) return false;
	if(!s->containsKey(k)) return false;

	// Create the new track object we will be adding.

	QString p = s->getAbsolutePath(k);

	CSIPodTrack *track = CSIPodTrack::createTrackFromFile(p);
	if(track == NULL)
	{
#ifdef CUTESYNC_DEBUG
std::cout << "Error creating new track object.\n";
#endif

		return false;
	}

	// Apply sorting options to the track.
	track->applySortOptions(caselessSort, ignorePrefixes);

	/*
	 * Since we are doing itdb_cp_track_to_ipod before adding it to itdb,
	 * we need to set this.
	 */

	track->getTrack()->itdb = itdb;

	// Try to set the track's artwork, if it is enabled.

	if(getAlbumArtworkEnabled())
	{
		gpointer cover = getTrackCoverArt(s, k);
		if(cover != NULL)
		{
			if(!itdb_track_set_thumbnails_from_pixbuf(
				track->getTrack(), cover))
			{
#ifdef CUTESYNC_DEBUG
std::cout << "Error setting track thumbnail: " <<
	s->getAbsolutePath(k).toLatin1().data() << "\n";
#endif
			}
		}
	}

	// Copy the track to the iPod.

	GError *error = NULL;
//itdb_cp_track_to_ipod @ itdb_itunesdb.c:7563
	if(!itdb_cp_track_to_ipod(track->getTrack(),
		p.toUtf8().data(), &error))
	{
		if(error != NULL)
		{
#ifdef CUTESYNC_DEBUG
std::cout << "Error copying track to iPod: " << error->message << "\n";
#endif

			g_error_free(error);
			error = NULL;
		}
		else
		{
#ifdef CUTESYNC_DEBUG
std::cout << "Unknown error copying track to iPod!\n";
#endif
		}

		delete track;
		return false;
	}

	// Add track to the ITDB, the MPL, our lists, and set ourself modified.

	itdb_track_add(itdb, track->getTrack(), -1);
	itdb_playlist_add_track(itdb_playlist_mpl(itdb),
		track->getTrack(), -1);
	addTrack(track);
	setModified(true);

	return true;
}

/*!
 * This function tries to load the appropriate cover art for the given source
 * track. This is used to set the cover art appropriately when the track is,
 * e.g., being copied into this collection.
 *
 * We search the following locations (in order) for appropriate artwork:
 *     - The tag of the track itself.
 *     - "cover.(image extension)" in the same directory as the track.
 *     - "folder.(image extension)" in the same directory as the track.
 *
 * Note that the filenames we search for are case-insensitive - "COVER" works
 * as well as "cover". If none of these are present, then this function simply
 * returns NULL instead.
 *
 * \param s The source collection.
 * \param k The key for the track in the source collection.
 * \return A GdkPixbuf object of the artwork, or NULL if it cannot be found.
 */
gpointer CSIPodCollection::getTrackCoverArt(
	const CSAbstractCollection *s, const QString &k)
{
	gpointer pixbuf = NULL;
	QString path;
	QDir pdir;
	GError *error = NULL;

	// Do some sanity checks.

	if(itdb == NULL) return NULL;
	if(!s->containsKey(k)) return NULL;

	path = s->getAbsolutePath(k);
	{
		QFileInfo f(path);
		pdir = f.dir();
	}

	// Try to retrieve the cover art embedded in the file itself, if any.

	{
		CSFileTypeResolver resolv;
		CSTaggedFile t(path, resolv, false);

		pixbuf = t.getCoverArtwork();
	}

	// If that didn't work, look for cover.*

	if(pixbuf == NULL)
	{
		QStringList entries = pdir.entryList(
			QStringList("cover.*"), QDir::Files);

		if(entries.size() == 1)
		{
			pixbuf = gdk_pixbuf_new_from_file(
				pdir.absoluteFilePath(entries.first())
				.toLatin1().data(), &error);

			if(error != NULL)
			{
				g_error_free(error);

				error = NULL;
				pixbuf = NULL;
			}
		}
	}

	// If that didn't work either, look for folder.*

	if(pixbuf == NULL)
	{
		QStringList entries = pdir.entryList(
			QStringList("folder.*"), QDir::Files);

		if(entries.size() == 1)
		{
			pixbuf = gdk_pixbuf_new_from_file(
				pdir.absoluteFilePath(
				entries.first()).toLatin1().data(), &error);

			if(error != NULL)
			{
				g_error_free(error);

				error = NULL;
				pixbuf = NULL;
			}
		}
	}

	// Return whatever pixbuf object we've managed to find.

	return pixbuf;
}

/*!
 * This function tries to read our collection options from the current
 * collection, or sets them to our normal defaults if they haven't ever been
 * set before.
 */
void CSIPodCollection::refreshCollectionOptions()
{
	if(itdb != NULL)
	{

		QFile optionsFile(getMountPoint() +
			"/iPod_Control/iTunes/CuteSync.prefs");

		if(optionsFile.exists() &&
			optionsFile.open(QIODevice::ReadOnly))
		{
			char options[16];
			if(optionsFile.read(options, 16) != 16)
			{
				optionsFile.close();
				optionsFile.remove();

				caselessSort = true;
				ignorePrefixes = true;

				optionsModified = true;
			}

			optionsFile.close();

			caselessSort   = (options[0] & 0x80) > 0;
			ignorePrefixes = (options[0] & 0x40) > 0;

			optionsModified = false;
		}
		else
		{
			caselessSort = true;
			ignorePrefixes = true;

			optionsModified = true;
		}
	}
}

/*!
 * This slot handles one of our configuration widgets requesting that its
 * current state be applied to our collection.
 */
void CSIPodCollection::doConfigurationApply()
{ /* SLOT */

	CSIPodCollectionConfigWidget *w =
		dynamic_cast<CSIPodCollectionConfigWidget *>(sender());

	if(w != NULL)
	{
		setSaveOnExit(w->getSaveState());
		setAlbumArtworkEnabled(w->getAlbumArtworkState());
		setCaseInsensitiveSort(w->getCaseSortState());
		setIgnoreCommonPrefixes(w->getIgnoreCommonPrefixState());
	}

}

/*!
 * This slot handles our configuration widget requesting a reset by resetting
 * its options to our currently stored values.
 */
void CSIPodCollection::doConfigurationReset()
{ /* SLOT */

	CSIPodCollectionConfigWidget *w =
		dynamic_cast<CSIPodCollectionConfigWidget *>(sender());

	if(w != NULL)
	{
		w->setSaveState(isSavedOnExit());
		w->setAlbumArtworkState(getAlbumArtworkEnabled());
		w->setCaseSortState(getCaseInsensitiveSort());
		w->setIgnoreCommonPrefixState(getIgnoreCommonPrefixes());
	}

}
