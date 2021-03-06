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

#include "track.h"

/*!
 * This is our default destructor which cleans up & destroys our object.
 */
CSTrack::~CSTrack()
{
}

/*!
 * This is a utility function that converts a length in seconds to a string fit
 * to be displayed for the user. For instance, if s = 172, the resulting string
 * would be: 2:52.
 *
 * \param s A time length, in seconds.
 * \return A nicely formatted string version of the given time.
 */
QString CSTrack::getLengthDisplay(int s)
{
	int h=0, m=0;

	if(s < 0) return "";

	m = (s / 60);
	s -= (m * 60);

	h = (m / 60);
	m -= (h * 60);

	QString r = (h > 0)
		? QString("%1:%2:%3").arg(h).arg(m, 2, 10, QChar('0'))
			.arg(s, 2, 10, QChar('0'))
		: QString("%1:%2").arg(m).arg(s, 2, 10, QChar('0'));

	return r;
}

/*!
 * This operator tests if this track is equivalent to a given other track. We
 * determine this by comparing the track hashes, which should uniquely identify
 * a track.
 *
 * \param o The other track to compare ourself to.
 * \return True if we are equal, or false otherwise.
 */
bool CSTrack::operator==(const CSTrack &o) const
{
	return ( getHash() == o.getHash() );
}

/*!
 * This function returns our current "track hash." This is not necessarily a
 * normal hexadecimal hash value; rather, it is just a string representing our
 * attributes which should uniquely identify this track.
 *
 * \return Our track's current "track hash."
 */
QString CSTrack::getHash() const
{
	QString r = "";

	r += getGenre();
	r += getArtist();
	r += getAlbum();
	r += getTitle();
	r += QString::number(getYear());
	r += QString::number(getTrackNumber());
	r += QString::number(getTrackCount());
	r += QString::number(getCDNumber());
	r += QString::number(getSize());
	r += QString::number(getLength());

	return r;
}

/*!
 * This function returns the value of the attribute indicated by the given
 * collection column. This makes it easy to display our track descriptors in a
 * table view.
 *
 * \param c The column desired.
 * \return The desired data.
 */
QVariant CSTrack::getColumn(CSAbstractCollection::Column c) const
{
	switch(c)
	{
		case CSAbstractCollection::Artist:
			return QVariant(getArtist());

		case CSAbstractCollection::Album:
			return QVariant(getAlbum());

		case CSAbstractCollection::Title:
			return QVariant(getTitle());

		case CSAbstractCollection::DiscNumber:
			return QVariant(getCDNumber());

		case CSAbstractCollection::TrackNumber:
			return QVariant(getTrackNumber());

		case CSAbstractCollection::TrackCount:
			return QVariant(getTrackCount());

		case CSAbstractCollection::Length:
			return QVariant(getLength());

		case CSAbstractCollection::Year:
			return QVariant(getYear());
	};

	return QVariant(QVariant::Invalid);
}
