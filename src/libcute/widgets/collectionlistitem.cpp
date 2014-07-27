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

#include "collectionlistitem.h"

#include "libcute/collections/abstractcollection.h"

/*!
 * This constructor creates a new collection list item which represents the
 * given collection, with the given parent object.
 *
 * \param c The collection this item will represent.
 * \param p Our parent object.
 */
CSCollectionListItem::CSCollectionListItem(CSAbstractCollection *c,
	QObject *p)
	: QObject(p), collection(NULL)
{
	setCollection(c);
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSCollectionListItem::~CSCollectionListItem()
{
}

/*!
 * This function returns a pointer to the collection our object is
 * representing.
 *
 * \return The collection we're representing.
 */
CSAbstractCollection *CSCollectionListItem::getCollection()
{
	return collection;
}

/*!
 * This function sets the collection our object is representing, disconnecting
 * any old collections's signals from this object.
 *
 * \param c The new collection to represent.
 */
void CSCollectionListItem::setCollection(CSAbstractCollection *c)
{
	if(collection != NULL)
		QObject::disconnect(collection, 0, this, 0);

	collection = c;

	Q_EMIT dataChanged(QVector<int>());
}
