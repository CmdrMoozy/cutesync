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

#include <QMutex>
#include <QMutexLocker>

#include "libcute/defines.h"
#include "libcute/collections/abstractcollection.h"
#include "libcute/collections/collectiontyperesolver.h"
#include "libcute/thread/pausablethread.h"

/*!
 * This is our default constructor, which initializes a new thread pool with
 * the given parent object.
 *
 * \param p Our parent object.
 */
CSCollectionThreadPool::CSCollectionThreadPool(QObject *p)
	: QObject(p), interruptible(true)
{
	controlMutex = new QMutex(QMutex::NonRecursive);

	// Create a new thread, as well as our collection type resolver.

	thread = new CSPausableThread(this);
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
		CSAbstractCollection *)), this, SLOT(doCollectionCreated(
		CSAbstractCollection *)));
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CSCollectionThreadPool::~CSCollectionThreadPool()
{
	delete resolver;
}

/*!
 * This function returns whether or not our thread pool's currently executing
 * job is easily interruptible. Jobs that aren't interruptible might include
 * things like synchronizing collections, which can leave the destination
 * collection in a semi-broken state if it is interrupted.
 *
 * \return Whether our not our current job is interruptible.
 */
bool CSCollectionThreadPool::isInterruptible()
{
	controlMutex->lock();
	bool i = interruptible;
	controlMutex->unlock();

	return i;
}

/*!
 * This function pauses our worker thread(s). See our CSPausableThread class
 * for more details. The thread(s) can later be resumed with our resume()
 * function.
 */
void CSCollectionThreadPool::pause()
{
	thread->pause();
}

/*!
 * This function resumes our worker thread(s). See our CSPausableThread class
 * for more details.
 */
void CSCollectionThreadPool::resume()
{
	thread->resume();
}

/*!
 * This function stops all of our threads gracefully. Note that this function
 * will block until all of the threads have stopped.
 *
 * Note that, in certain circumstances, we prompt the user and allow them to
 * abort the stop operation; hence our return value.
 *
 * \return True if we stopped gracefully, or false otherwise.
 */
bool CSCollectionThreadPool::stopGracefully()
{
	bool i = isInterruptible();

	if(!i)
	{
#pragma message "TODO - Prompt the user to interrupt this dangerous job."
	}

	Q_EMIT(interruptAllJobs());

	thread->quit();
	thread->wait();

	return true;
}

/*!
 * This function sets whether or not our current job is interruptible. Note
 * that this function is completely thread-safe.
 *
 * \param i The new interruptible status for our object.
 */
void CSCollectionThreadPool::setInterruptible(bool i)
{
	QMutexLocker locker(controlMutex);
	interruptible = i;
}

/*!
 * This slot handles a request to create a new collection using the given
 * details. This schedules a new collection action to be executed in a worker
 * thread at the end of any current event queue.
 *
 * \param n The name of the new collection.
 * \param p The path of the collection.
 * \param s Whether or not the collection should be saved on exit.
 */
void CSCollectionThreadPool::newCollection(const QString &n,
	const QString &p, bool s)
{ /* SLOT */

	Q_EMIT(startNew(n, p, s));

}

/*!
 * This function handles a new collection being created by connecting to some
 * of its signals / slots.
 *
 * \param c The newly-created collection.
 */
void CSCollectionThreadPool::doCollectionCreated(CSAbstractCollection *c)
{ /* SLOT */

	/*
	 * Connect the signal/slot we use to interrupt collection actions. Note
	 * that this MUST be a Qt::DirectConnection, or otherwise the
	 * interruption won't actually occur until the collection's thread
	 * returns control to the event dispatch loop (i.e., after the job is
	 * already completely done anyway).
	 */

	QObject::connect(this, SIGNAL(interruptAllJobs()),
		c, SLOT(setInterrupted()), Qt::DirectConnection);

	Q_EMIT collectionCreated(c);

}

/*!
 * This function handles a new job being started by setting our interruptible
 * status to the reported value.
 *
 * \param j The job's description (UNUSED).
 * \param i Whether or not the job is interruptible.
 */
void CSCollectionThreadPool::doJobStarted(const QString &UNUSED(j), bool i)
{ /* SLOT */

	setInterruptible(i);

}

/*!
 * This function handles a job being finished by resetting our interruptible
 * state back to true.
 *
 * \param r The result of the job (UNUSED).
 */
void CSCollectionThreadPool::doJobFinished(const QString &UNUSED(r))
{ /* SLOT */

	setInterruptible(true);

}
