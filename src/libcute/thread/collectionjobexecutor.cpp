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

#include "collectionjobexecutor.h"

#include <QThread>

#include "libcute/collections/abstractcollection.h"

/*!
 * This is our default constructor, which creates a new instance of our job
 * executor running on the given thread.
 *
 * \param t The thread to run jobs on.
 */
CSCollectionJobExecutor::CSCollectionJobExecutor(QThread *t)
	: QObject()
{
	moveToThread(t);
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSCollectionJobExecutor::~CSCollectionJobExecutor()
{
}

/*!
 * This function starts a new collection synchronization job in our worker
 * thread.
 *
 * \param s The source collection to sync from.
 * \param d The destination collection to sync to.
 */
void CSCollectionJobExecutor::syncCollections(
	CSAbstractCollection *s, CSAbstractCollection *d)
{ /* SLOT */

	Q_ASSERT(thread() == s->thread());
	Q_ASSERT(s->thread() == d->thread());

	d->syncFrom(s);

}
