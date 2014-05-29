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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_DIR_TRACK_H
#define INCLUDE_LIBCUTE_COLLECTIONS_DIR_TRACK_H

#include "libcute/collections/Track.h"

/*!
 * \brief This class provides a track descriptor for tracks loaded from flat files.
 *
 * Track attributes are retrieved via some combination of custom MMIO code and taglib. This is fairly fast, but
 * still quite slow compared to reading cached data. Because of this, it is reccommended you serialize these
 * track descriptors where applicable, and try to avoid calling refresh() if possible.
 */
class CuteSyncDirTrack : public CuteSyncTrack
{
	public:
		CuteSyncDirTrack(const QString &p);
		virtual ~CuteSyncDirTrack();

		virtual QString getPath() const;
		virtual QString getTitle() const;
		virtual QString getArtist() const;
		virtual QString getAlbum() const;
		virtual QString getComment() const;
		virtual QString getGenre() const;
		virtual QString getAlbumArtist() const;
		virtual QString getComposer() const;
		virtual int getYear() const;
		virtual int getTrackNumber() const;
		virtual int getTrackCount() const;
		virtual int getCDNumber() const;
		virtual int getLength() const;
		virtual int getBitrate() const;
		virtual int getSamplerate() const;
		virtual int64_t getSize() const;
		virtual QDateTime getModifyTime() const;

		virtual QByteArray serialize() const;
		virtual void unserialize(const QByteArray &d);

		virtual bool refresh();

	private:
		QString path, title, artist, album, comment, genre, albumartist, composer;
		int year, trackNumber, trackCount, cdNumber, length, bitrate, samplerate;
		int64_t size;
		QDateTime modifyTime;
};

#endif
