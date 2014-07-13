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

#include "collectionlistwidget.h"

#include "libcute/collections/AbstractCollection.h"
#include "libcute/collections/DirCollection.h"
#include "libcute/collections/IPodCollection.h"
#include "cutesync/widgets/collectiondelegate.h"
#include "libcute/widgets/collectionmodel.h"

/*!
 * This is our default constructor, which creates a new collection list with
 * the given parent widget.
 *
 * \param p Our parent widget.
 */
CSCollectionListWidget::CSCollectionListWidget(QWidget *p)
	: CSIconListWidget(p), mdl(NULL)
{
	delegate = new CSCollectionDelegate(this);
	setItemDelegate(delegate);

	QObject::connect(this, SIGNAL(clicked(const QModelIndex &)),
		this, SLOT(doCollectionClicked(const QModelIndex &)));
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSCollectionListWidget::~CSCollectionListWidget()
{
}

/*!
 * This function returns a pointer to the model we are currently using. Note
 * that, by definition, you can cast this to a CSCollectionModel pointer, as
 * that is all we accept. Also note that this function, unlike the function on
 * our superclass, CAN RETURN NULL. By default, our class has no model until
 * you provide us with one.
 *
 * \return A pointer to our current model.
 */
QAbstractItemModel *CSCollectionListWidget::model() const
{
	return mdl;
}

/*!
 * This function sets the model we should be displaying. Note that this only
 * accepts a CSCollectionModel - this class is designed to use those, and not
 * any generic QAbstractItemModel.
 *
 * \param m The model we should use.
 */
void CSCollectionListWidget::setCollectionModel(CSCollectionModel *m)
{
	if(mdl != NULL) mdl->disconnect(this);
	mdl = m;

	QObject::connect(mdl, SIGNAL(rowEnabledChanged(const QModelIndex &)),
		this, SLOT(doRowEnabledChanged(const QModelIndex &)));

	delegate->setModel(mdl);
	setModel(mdl);
}

/*!
 * We make our superclass's implementation of this private to force users to
 * use setCollectionModel() instead. This overridden function just does the
 * same thing as our superclass's implementation, except it is private.
 *
 * \param m The new model we should be using.
 */
void CSCollectionListWidget::setModel(QAbstractItemModel *m)
{
	CSIconListWidget::setModel(m);
}

/*!
 * This function handles a collection being clicked by emitting a signal
 * letting anyone using this widget know about it. Note that if the clicked
 * collection is not enabled, then we clear the selection, since disabled items
 * are not supposed to be used.
 *
 * \param i The index of the collection that was clicked.
 */
void CSCollectionListWidget::doCollectionClicked(const QModelIndex &i)
{ /* SLOT */

	if(mdl != NULL)
	{
		CuteSyncAbstractCollection *c = mdl->collectionAt(i.row());

		if( (c != NULL) && (c->isEnabled()) )
			Q_EMIT selectionChanged(mdl->collectionAt(i.row()));
		else
			clearSelection();
	}

}

/*!
 * This function handles the visibility of a specific row being changed by
 * either showing or hiding it, depending on its new visibility state.
 *
 * \param i The index of the item in the model whose state has changed.
 */
void CSCollectionListWidget::doRowEnabledChanged(const QModelIndex &i)
{ /* SLOT */

	// Just update the item so it is painted correctly as per our delegate.
	update(i);

}
