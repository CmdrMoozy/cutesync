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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_TRACK_H
#define INCLUDE_LIBCUTE_COLLECTIONS_TRACK_H

#include <cstdint>

#include <QString>
#include <QDateTime>

#include "libcute/collections/AbstractCollection.h"

/*!
 * \brief This class provides a base class for a track descriptor.
 *
 * It should be subclassed for your particular collection type, to provide collection-specific
 * functionality, on top of the base functionality provided here.
 */
class CuteSyncTrack
{
	public:
		virtual ~CuteSyncTrack();

		static QString getLengthDisplay(int s);

		virtual bool operator==(const CuteSyncTrack &o) const;

		virtual QString getPath() const = 0;
		virtual QString getTitle() const = 0;
		virtual QString getArtist() const = 0;
		virtual QString getAlbum() const = 0;
		virtual QString getComment() const = 0;
		virtual QString getGenre() const = 0;
		virtual QString getAlbumArtist() const = 0;
		virtual QString getComposer() const = 0;
		virtual int getYear() const = 0;
		virtual int getTrackNumber() const = 0;
		virtual int getTrackCount() const = 0;
		virtual int getCDNumber() const = 0;
		virtual int getLength() const = 0;
		virtual int getBitrate() const = 0;
		virtual int getSamplerate() const = 0;
		virtual int64_t getSize() const = 0;
		virtual QDateTime getModifyTime() const = 0;

		QString getHash() const;
		QVariant getColumn(CuteSyncAbstractCollection::Column c) const;

		virtual QByteArray serialize() const = 0;
		virtual void unserialize(const QByteArray &d) = 0;

		virtual bool refresh() = 0;
};

#endif
