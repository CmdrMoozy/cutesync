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

#include "collectiondelegate.h"

#include <QStyleOptionViewItemV4>

#include "libcute/collections/abstractcollection.h"
#include "libcute/widgets/collectionmodel.h"

/*!
 * This constructor creates a new collection delegate with the given parent
 * object.
 *
 * \param p Our delegate's parent object.
 */
CSCollectionDelegate::CSCollectionDelegate(QObject *p)
	: QStyledItemDelegate(p), model(NULL)
{
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CSCollectionDelegate::~CSCollectionDelegate()
{
}

/*!
 * This function returns a pointer to the model our delegate is currently
 * operating on.
 *
 * \return Our current collection model.
 */
const CSCollectionModel *CSCollectionDelegate::getModel() const
{
	return model;
}

/*!
 * This function sets the collection model our delegate should be working with.
 * This is necessary so the delegate can detect a given collection's state in
 * order to draw it appropriately. If you give us a NULL model, or an invalid
 * one, you can expect us to simply draw nothing.
 *
 * \param m The model to work with.
 */
void CSCollectionDelegate::setModel(const CSCollectionModel *m)
{
	model = m;
}

/*!
 * We implement our superclass's paint method to paint the actual collections.
 * This detects a collection's state, and then paints it appropriately (e.g.,
 * greys it out if it is disabled).
 *
 * \param p The painter to paint with.
 * \param o The style descriptor to work with.
 * \param i The index of the item that is to be painted.
 */
void CSCollectionDelegate::paint(QPainter *p,
	const QStyleOptionViewItem &o, const QModelIndex &i) const
{
	if(model != NULL)
	{
		CSAbstractCollection *c = model->collectionAt(i.row());

		if(c != NULL)
		{
			QStyleOptionViewItemV4 options(o);

			if(!c->isEnabled())
				options.state &= ~QStyle::State_Enabled;

			QStyledItemDelegate::paint(p, options, i);
		}
	}
}
