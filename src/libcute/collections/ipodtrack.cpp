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

#include "ipodtrack.h"

#include "libcute/defines.h"
#include "libcute/tags/filetyperesolver.h"
#include "libcute/tags/taggedfile.h"

/*!
 * This function constructs a new Itdb_Track from a normal flat file. If for
 * whatever reason we are unable to read the file given, NULL is returned
 * instead.
 *
 * Note that it is up to the caller to ensure that the resulting pointer gets
 * free()'d appropriately when it is no longer needed.
 *
 * Also note that we DO NOT SET track->itdb. You may need to set this manually
 * if you are going to be doing things with the resulting track BEFORE calling
 * itdb_track_add on it.
 *
 * \param p The path to the file we are looking at.
 * \return A new track, or NULL on failure.
 */
CSIPodTrack *CSIPodTrack::createTrackFromFile(const QString &p)
{
	Itdb_Track *track = itdb_track_new();

	CSFileTypeResolver resolver;
	CSTaggedFile f(p, resolver);

	track->title = g_strdup(f.getTitle().toUtf8().data());
	track->album = g_strdup(f.getAlbum().toUtf8().data());
	track->artist = g_strdup(f.getArtist().toUtf8().data());
	track->genre = g_strdup(f.getGenre().toUtf8().data());
	track->comment = g_strdup(f.getComment().toUtf8().data());
	track->composer = g_strdup(f.getComposer().toUtf8().data());
	track->albumartist = g_strdup(f.getAlbumArtist().toUtf8().data());
	track->keywords = g_strdup(f.getKeywords().toUtf8().data());

	track->size = static_cast<gint32>(f.getSize());

	// Multiply by 1,000 to get milliseconds.
	track->tracklen = static_cast<gint32>(f.getTrackLength() * 1000);

	track->cd_nr = static_cast<gint32>(f.getDiscNumber());
	track->track_nr = static_cast<gint32>(f.getTrackNumber());
	track->tracks = static_cast<gint32>(f.getTrackCount());
	track->bitrate = static_cast<gint32>(f.getBitrate());
	track->samplerate = static_cast<guint16>(f.getSampleRate());
	track->year = static_cast<gint32>(f.getYear());

	// 0x 00 00 00 01 means AUDIO.
	track->mediatype  = static_cast<guint32>(0x00000001);

	// E.g., "MP3-File"
	track->filetype = g_strdup(f.getSuffix().toUpper()
		.append("-File").toUtf8().data());

	return new CSIPodTrack(track);
}

/*!
 * This constructor creates a new iPod track descriptor based on the given
 * existing libgpod track object. Note that we take ownership of this track
 * object; we will call itdb_track_free on it, so DO NOT do it yourself!
 *
 * \param t The track object we will represent.
 */
CSIPodTrack::CSIPodTrack(Itdb_Track *t)
	: track(t)
{
}

/*!
 * This is our default destructor, which frees our libgpod track (if any) and
 * cleans up our object.
 */
CSIPodTrack::~CSIPodTrack()
{
	if(track != NULL)
		itdb_track_free(track);
}

/*!
 * This function returns a pointer to the libgpod track we are representing.
 * Note that, depending on what you gave the constructor, this function can
 * return NULL.
 *
 * Also note that these track descriptors ALWAYS maintain ownership of their
 * libgpod track objects; this class will handle freeing memory appropriately,
 * so you should never do that manually on the pointer returned by this
 * function.
 *
 * \return A pointer to the libgpod track we represent.
 */
Itdb_Track *CSIPodTrack::getTrack() const
{
	return track;
}

/*!
 * This function returns the absolute path to the track we represent. This is
 * going to be a path on the iPod device.
 *
 * \return Our track's absolute path.
 */
QString CSIPodTrack::getPath() const
{
#pragma message "TODO - Implement me"

	return QString("");
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's title.
 */
QString CSIPodTrack::getTitle() const
{
	if(track != NULL)
		return QString::fromUtf8(track->title);

	return QString("");
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's artist.
 */
QString CSIPodTrack::getArtist() const
{
	if(track != NULL)
		return QString::fromUtf8(track->artist);

	return QString("");
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's album.
 */
QString CSIPodTrack::getAlbum() const
{
	if(track != NULL)
		return QString::fromUtf8(track->album);

	return QString("");
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's comment.
 */
QString CSIPodTrack::getComment() const
{
	if(track != NULL)
		return QString::fromUtf8(track->comment);

	return QString("");
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's genre.
 */
QString CSIPodTrack::getGenre() const
{
	if(track != NULL)
		return QString::fromUtf8(track->genre);

	return QString("");
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's album artist.
 */
QString CSIPodTrack::getAlbumArtist() const
{
	if(track != NULL)
		return QString::fromUtf8(track->albumartist);

	return QString("");
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's composer.
 */
QString CSIPodTrack::getComposer() const
{
	if(track != NULL)
		return QString::fromUtf8(track->composer);

	return QString("");
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's year.
 */
int CSIPodTrack::getYear() const
{
	if(track != NULL)
		return static_cast<int>(track->year);

	return 0;
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's track number on the CD it is a part of.
 */
int CSIPodTrack::getTrackNumber() const
{
	if(track != NULL)
		return static_cast<int>(track->track_nr);

	return 0;
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return The number of tracks on the CD this track is a part of.
 */
int CSIPodTrack::getTrackCount() const
{
	if(track != NULL)
		return static_cast<int>(track->tracks);

	return 0;
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Which CD number in a set this track is a part of.
 */
int CSIPodTrack::getCDNumber() const
{
	if(track != NULL)
		return static_cast<int>(track->cd_nr);

	return 0;
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return The length of the track, in seconds.
 */
int CSIPodTrack::getLength() const
{
	if(track != NULL)
		return static_cast<int>(track->tracklen / 1000);

	return 0;
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's bitrate (exact for CBR, an average for VBR).
 */
int CSIPodTrack::getBitrate() const
{
	if(track != NULL)
		return static_cast<int>(track->bitrate);

	return 0;
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's sample rate, in hertz.
 */
int CSIPodTrack::getSamplerate() const
{
	if(track != NULL)
		return static_cast<int>(track->samplerate);

	return 0;
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's filesize, in bytes.
 */
int64_t CSIPodTrack::getSize() const
{
	if(track != NULL)
		return static_cast<int64_t>(track->size);

	return 0;
}

/*!
 * An attribute accessor function. This will retrieve tag information using
 * libgpod, so no actual reading of the media file itself is done. This means
 * that these functions are very fast.
 *
 * \return Our track's last modified time.
 */
QDateTime CSIPodTrack::getModifyTime() const
{
	if(track != NULL)
		return QDateTime::fromTime_t(track->time_modified);

	return QDateTime();
}

/*!
 * iPod tracks are not serializable. This is because, since all of the
 * information about them is stored in the iTunes DB on an iPod, reading an
 * entire collection of tracks is VERY fast - therefore, there is really no
 * purpose behind serializing track descriptors to read them later.
 *
 * \return An empty byte array.
 */
QByteArray CSIPodTrack::serialize() const
{
	return QByteArray();
}

/*!
 * iPod tracks are not serializable. This is because, since all of the
 * information about them is stored in the iTunes DB on an iPod, reading an
 * entire collection of tracks is VERY fast - therefore, there is really no
 * purpose behind serializing track descriptors to read them later.
 *
 * As such, this function does nothing.
 *
 * \return A byte array to unserialize (unused).
 */
void CSIPodTrack::unserialize(const QByteArray &UNUSED(d))
{
}

/*!
 * This function refreshes the metadata stored by our track descriptor. iPod
 * tracks, because of the way the iTunes DB on iPod devices works, do not cache
 * metadata, so this function does effectively nothing.
 *
 * \return True, indicating success.
 */
bool CSIPodTrack::refresh()
{
	return true;
}

/*!
 * This function updates our track's sort fields using the given sort options.
 *
 * \param c True means sorting should be case-insensitive.
 * \param i True means ignore common prefixes like 'The ...'.
 */
void CSIPodTrack::applySortOptions(bool c, bool i)
{
	if(track != NULL)
	{
		QString sTitle = getTitle();
		QString sArtist = getArtist();
		QString sAlbum = getAlbum();
		QString sAlbumArtist = getAlbumArtist();
		QString sComposer = getComposer();

		// If ignoring common prefixes, remove them from certain fields.

		if(i)
		{
			/*
			 * The prefixes we ignore are:
			 *
			 *     "The ..."
			 *
			 * In the fields:
			 *
			 *     Artist
			 *     Album
			 */

			if(sArtist.startsWith("The ", Qt::CaseInsensitive))
				sArtist = sArtist.right(sArtist.length() - 4);

			if(sAlbum.startsWith("The ", Qt::CaseInsensitive))
				sAlbum = sAlbum.right(sAlbum.length() - 4);

			if(sAlbumArtist.startsWith("The ", Qt::CaseInsensitive))
			{
				sAlbumArtist = sAlbumArtist.right(
					sAlbumArtist.length() - 4);
			}

			if(sComposer.startsWith("The ", Qt::CaseInsensitive))
			{
				sComposer = sComposer.right(
					sComposer.length() - 4);
			}
		}

		// If sorting is case-insensitive, make everything uppercase.

		if(c)
		{
			sTitle = sTitle.toUpper();
			sArtist = sArtist.toUpper();
			sAlbum = sAlbum.toUpper();
			sAlbumArtist = sAlbumArtist.toUpper();
			sComposer = sComposer.toUpper();
		}

		// Set the sort fields in the actual iTunes DB track.

		track->sort_artist = g_strdup(sArtist.toUtf8().data());
		track->sort_title = g_strdup(sTitle.toUtf8().data());
		track->sort_album = g_strdup(sAlbum.toUtf8().data());
		track->sort_albumartist = g_strdup(
			sAlbumArtist.toUtf8().data());
		track->sort_composer = g_strdup(sComposer.toUtf8().data());
	}
}
