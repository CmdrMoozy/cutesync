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

#include "collectionthreadpool.h"

#include <QThread>

#include "libcute/collections/abstractcollection.h"
#include "libcute/collections/collectiontyperesolver.h"
#include "libcute/widgets/collectionlistitem.h"

CSCollectionThreadPool::CSCollectionThreadPool(QObject *p)
	: QObject(p)
{
	// Create a new thread, as well as our collection type resolver.

	thread = new QThread(this);
	thread->start();

	resolver = new CSCollectionTypeResolver();
	resolver->moveToThread(thread);

	// Connect the collection type resolver's progress signals.

	QObject::connect(resolver, SIGNAL(jobStarted(const QString &)),
		this, SIGNAL(jobStarted(const QString &)));
	QObject::connect(resolver, SIGNAL(progressLimitsUpdated(int, int)),
		this, SIGNAL(progressLimitsUpdated(int, int)));
	QObject::connect(resolver, SIGNAL(progressUpdated(int)),
		this, SIGNAL(progressUpdated(int)));
	QObject::connect(resolver, SIGNAL(jobFinished(const QString &)),
		this, SIGNAL(jobFinished(const QString &)));

	// Connect our action signals to the collection type resolver.

	QObject::connect(this, SIGNAL(startNew(const QString &,
		const QString &, bool)), resolver, SLOT(newCollection(
		const QString &, const QString &, bool)));
}

CSCollectionThreadPool::~CSCollectionThreadPool()
{
	delete resolver;
}

void CSCollectionThreadPool::newCollection(const QString &n,
	const QString &p, bool s)
{ /* SLOT */

	Q_EMIT(startNew(n, p, s));

}
