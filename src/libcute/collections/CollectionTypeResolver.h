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
#define INCLUDE_LIBCUTE_COLLECTIONS_COLLECTION_TYPE_RESULVER_H

class QString;

class CuteSyncAbstractCollection;

/*!
 * \brief This class provides a way to determine the type of collection a directory contains.
 *
 * Using the createCollection() function, the user can provide simply a path and a name and get a
 * valid and correct collection pointer in return. This class supports fully all of the built-in
 * collection types; if you are subclassing CuteSyncAbstractCollection for a custom type, you should
 * also subclass this class to provide detection for it.
 */
class CuteSyncCollectionTypeResolver
{
	public:
		CuteSyncCollectionTypeResolver();
		virtual ~CuteSyncCollectionTypeResolver();

		virtual CuteSyncAbstractCollection *createCollection(const QString &n, const QString &p) const;
};

#endif