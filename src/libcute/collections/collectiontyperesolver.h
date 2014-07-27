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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_COLLECTION_TYPE_RESOLVER_H
#define INCLUDE_LIBCUTE_COLLECTIONS_COLLECTION_TYPE_RESOLVER_H

#include <QObject>

class QString;
class QByteArray;

class CSAbstractCollection;
class CSCollectionListItem;

/*!
 * \brief This class can determine the type of collection a path contains.
 *
 * Using the createCollection() function, the user can provide simply a path
 * and a name and get a valid and correct collection pointer in return. This
 * class supports fully all of the built-in collection types; if you are
 * subclassing CSAbstractCollection for a custom type, you should also subclass
 * this class to provide detection for it.
 */
class CSCollectionTypeResolver : public QObject
{
	Q_OBJECT

	public:
		CSCollectionTypeResolver(QObject *p = 0);
		virtual ~CSCollectionTypeResolver();

	private:
		CSAbstractCollection *createCollection(const QString &n,
			const QString &p) const;

	public Q_SLOTS:
		void newCollection(const QString &n,
			const QString &p, bool s);

	private Q_SLOTS:
		void doJobStarted(const QString &j, bool i);
		void doProgressLimitsUpdated(int min, int max);
		void doProgressUpdated(int p);
		void doJobFinished(const QString &r);

	Q_SIGNALS:
		void jobStarted(const QString &, bool);
		void progressLimitsUpdated(int, int);
		void progressUpdated(int);
		void jobFinished(const QString &);

		void collectionCreated(CSAbstractCollection *);
};

#endif
