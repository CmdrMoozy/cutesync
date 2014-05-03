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

#include "CuteSyncDirTrack.h"

#include <QFileInfo>
#include <QByteArray>
#include <QDataStream>

#include "CuteSyncDefines.h"
#include "tags/CuteSyncFileTypeResolver.h"
#include "tags/CuteSyncTaggedFile.h"

/*!
 * This constructor creates a new directory collection track from the given file path. Note that
 * this function doesn't automatically load track data; rather, you should be sure to call refresh()
 * before trying to access attributes.
 *
 * \param p The path where the desired track file is located.
 */
CuteSyncDirTrack::CuteSyncDirTrack(const QString &p)
	: path(p)
{
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CuteSyncDirTrack::~CuteSyncDirTrack()
{
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return The path to our track file.
 */
QString CuteSyncDirTrack::getPath() const
{
	return path;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's title.
 */
QString CuteSyncDirTrack::getTitle() const
{
	return title;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's artist.
 */
QString CuteSyncDirTrack::getArtist() const
{
	return artist;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's album.
 */
QString CuteSyncDirTrack::getAlbum() const
{
	return album;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's comment.
 */
QString CuteSyncDirTrack::getComment() const
{
	return comment;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's genre.
 */
QString CuteSyncDirTrack::getGenre() const
{
	return genre;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's album artist.
 */
QString CuteSyncDirTrack::getAlbumArtist() const
{
	return albumartist;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's composer.
 */
QString CuteSyncDirTrack::getComposer() const
{
	return composer;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's year.
 */
int CuteSyncDirTrack::getYear() const
{
	return year;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's track number on the CD it is a part of.
 */
int CuteSyncDirTrack::getTrackNumber() const
{
	return trackNumber;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return The number of tracks on the CD this track is a part of.
 */
int CuteSyncDirTrack::getTrackCount() const
{
	return trackCount;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Which CD number in a set this track is a part of.
 */
int CuteSyncDirTrack::getCDNumber() const
{
	return cdNumber;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return The length of the track, in seconds.
 */
int CuteSyncDirTrack::getLength() const
{
	return length;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's bitrate (exact for CBR, an average for VBR).
 */
int CuteSyncDirTrack::getBitrate() const
{
	return bitrate;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's sample rate, in hertz.
 */
int CuteSyncDirTrack::getSamplerate() const
{
	return samplerate;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's filesize, in bytes.
 */
int64_t CuteSyncDirTrack::getSize() const
{
	return size;
}

/*!
 * An attribute accessor function. This will return cached track information, so it is quite fast. If you believe the
 * track file may have changed, rendering our cached data out-of-date, you should call refresh() before accessing these
 * attributes.
 *
 * \return Our track's last modified time.
 */
QDateTime CuteSyncDirTrack::getModifyTime() const
{
	return modifyTime;
}

/*!
 * This function serializes our track's current cached data, and returns the resulting state as a byte array.
 * Reading track data from files is rather slow, so you are encouraged to serialize these track descriptors where
 * possible.
 *
 * \return Our descriptor's state, as a byte array.
 */
QByteArray CuteSyncDirTrack::serialize() const
{
	QByteArray obuf;
	QDataStream out(&obuf, QIODevice::ReadWrite);

	out.setVersion(SERIALIZATION_VERSION);
	if(out.status() != QDataStream::Ok) return QByteArray();

	// Write our version number.
	out << static_cast<qint32>(SERIALIZATION_VERSION);

	// Write our attributes!

	out << path;
	out << title;
	out << artist;
	out << album;
	out << comment;
	out << genre;
	out << static_cast<qint32>(year);
	out << static_cast<qint32>(trackNumber);
	out << static_cast<qint32>(trackCount);
	out << static_cast<qint32>(cdNumber);
	out << static_cast<qint32>(length);
	out << static_cast<qint32>(bitrate);
	out << static_cast<qint32>(samplerate);
	out << static_cast<qint64>(size);
	out << modifyTime;

	// Done.

	return obuf;
}

/*!
 * This function restores our track descriptor from a serialized state. Note that this doesn't automatically
 * re-read track information; you should probably call refresh() manually if you expect that the source file
 * has been modified.
 *
 * \param d The byte array containing a stored track descriptor state.
 */
void CuteSyncDirTrack::unserialize(const QByteArray &d)
{
	qint32 version;

	// Create our input stream.

	QDataStream in(d);
	if(in.status() != QDataStream::Ok) return;

	// Read our version.

	in >> version;
	if(version > SERIALIZATION_VERSION) return;

	in.setVersion(version);
	if(in.status() != QDataStream::Ok) return;

	// Read our string attributes!

	in >> path;
	in >> title;
	in >> artist;
	in >> album;
	in >> comment;
	in >> genre;

	// Read our numeric attributes.

	qint32 i32b;
	qint64 i64b;

	in >> i32b;
	year = static_cast<int>(i32b);

	in >> i32b;
	trackNumber = static_cast<int>(i32b);

	in >> i32b;
	trackCount = static_cast<int>(i32b);

	in >> i32b;
	cdNumber = static_cast<int>(i32b);

	in >> i32b;
	length = static_cast<int>(i32b);

	in >> i32b;
	bitrate = static_cast<int>(i32b);

	in >> i32b;
	samplerate = static_cast<int>(i32b);

	in >> i64b;
	size = static_cast<int64_t>(i64b);

	// Read our final attribute.

	in >> modifyTime;
}

/*!
 * This function refreshes our cached track information by reading it again from the source file. This is fairly slow,
 * so avoid calling this function unless you have a good reason to need an update.
 *
 * \return True on success, or false otherwise.
 */
bool CuteSyncDirTrack::refresh()
{
	CuteSyncFileTypeResolver resolver;
	CuteSyncTaggedFile f(getPath(), resolver);

	if( f.isNull() || (!f.hasAudioProperties()) )
		return false;

	path        = f.getAbsolutePath();
	title       = f.getTitle();
	artist      = f.getArtist();
	album       = f.getAlbum();
	comment     = f.getComment();
	genre       = f.getGenre();
	albumartist = f.getAlbumArtist();
	composer    = f.getComposer();
	year        = f.getYear();
	trackNumber = f.getTrackNumber();
	trackCount  = f.getTrackCount();
	cdNumber    = f.getDiscNumber();
	length      = f.getTrackLength();
	bitrate     = f.getBitrate();
	samplerate  = f.getSampleRate();
	size        = f.getSize();
	modifyTime  = QFileInfo(f.getAbsolutePath()).lastModified();

	return true;
}
