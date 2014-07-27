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
#include <QMutex>
#include <QMutexLocker>

#include "libcute/defines.h"
#include "libcute/collections/abstractcollection.h"
#include "libcute/collections/collectiontyperesolver.h"

CSCollectionThreadPool::CSCollectionThreadPool(QObject *p)
	: QObject(p), interruptible(true)
{
	interruptibleMutex = new QMutex(QMutex::NonRecursive);

	// Create a new thread, as well as our collection type resolver.

	thread = new QThread(this);
	thread->start();

	resolver = new CSCollectionTypeResolver();
	resolver->moveToThread(thread);

	// Connect the collection type resolver's progress signals.

	QObject::connect(resolver, SIGNAL(jobStarted(const QString &, bool)),
		this, SIGNAL(jobStarted(const QString &, bool)));
	QObject::connect(resolver, SIGNAL(progressLimitsUpdated(int, int)),
		this, SIGNAL(progressLimitsUpdated(int, int)));
	QObject::connect(resolver, SIGNAL(progressUpdated(int)),
		this, SIGNAL(progressUpdated(int)));
	QObject::connect(resolver, SIGNAL(jobFinished(const QString &)),
		this, SIGNAL(jobFinished(const QString &)));

	QObject::connect(resolver, SIGNAL(jobStarted(const QString &, bool)),
		this, SLOT(doJobStarted(const QString &, bool)));
	QObject::connect(resolver, SIGNAL(jobFinished(const QString &)),
		this, SLOT(doJobFinished(const QString &)));

	// Connect our action signals to the collection type resolver.

	QObject::connect(this, SIGNAL(startNew(const QString &,
		const QString &, bool)), resolver, SLOT(newCollection(
		const QString &, const QString &, bool)));

	// Connect the type resolver's other signals to our signals.

	QObject::connect(resolver, SIGNAL(collectionCreated(
		CSAbstractCollection *)), this, SIGNAL(collectionCreated(
		CSAbstractCollection *)));
}

CSCollectionThreadPool::~CSCollectionThreadPool()
{
	delete resolver;
}

bool CSCollectionThreadPool::isInterruptible()
{
	interruptibleMutex->lock();
	bool i = interruptible;
	interruptibleMutex->unlock();

	return i;
}

bool CSCollectionThreadPool::stopGracefully()
{
	bool i = isInterruptible();

	return true;
}

void CSCollectionThreadPool::setInterruptible(bool i)
{
	QMutexLocker locker(interruptibleMutex);
	interruptible = i;
}

void CSCollectionThreadPool::newCollection(const QString &n,
	const QString &p, bool s)
{ /* SLOT */

	Q_EMIT(startNew(n, p, s));

}

void CSCollectionThreadPool::doCollectionCreated(CSAbstractCollection *c)
{ /* SLOT */

//TODO - connect our "interrupt" signal to the collection's "setInterrupted()" slot

}

void CSCollectionThreadPool::doJobStarted(const QString &UNUSED(j), bool i)
{ /* SLOT */

	setInterruptible(i);

}

void CSCollectionThreadPool::doJobFinished(const QString &UNUSED(r))
{ /* SLOT */

	setInterruptible(true);

}
