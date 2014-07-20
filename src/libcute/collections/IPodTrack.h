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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_IPOD_TRACK_H
#define INCLUDE_LIBCUTE_COLLECTIONS_IPOD_TRACK_H

#include "libcute/collections/track.h"

#include <cstdint>

extern "C" {
	#include <glib.h>
	#include <gpod-1.0/gpod/itdb.h>
}

/*!
 * \brief This class provides a track descriptor for tracks on an iPod device.
 *
 * Track attributes are retrieved via libgpod, rather than by reading the tags
 * from the track files themselves.
 *
 * Also note that this track descriptor doesn't do serialization: because of
 * how fast it is to read track data fro the iTunes DB file on iPod devices, it
 * isn't worthwhile to serialize the data encapsulated by one of these objects.
 */
class CuteSyncIPodTrack : public CSTrack
{
	public:
		static CuteSyncIPodTrack *createTrackFromFile(
			const QString &p);

		CuteSyncIPodTrack(Itdb_Track *t);
		virtual ~CuteSyncIPodTrack();

		Itdb_Track *getTrack() const;

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

		void applySortOptions(bool c, bool i);

	private:
		Itdb_Track *track;
};

#endif
