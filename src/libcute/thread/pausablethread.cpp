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

#include "pausablethread.h"

#include <QMutex>
#include <QMutexLocker>

/*!
 * This is our default constructor, which creates a new pausable thread
 * instance with the given parent object.
 *
 * \param parent The thread's parent object.
 */
CSPausableThread::CSPausableThread(QObject *parent)
	: QThread(parent), paused(false), resumeScheduled(false)
{
	controlMutex = new QMutex(QMutex::NonRecursive);

	QObject::connect(this, SIGNAL(finished()),
		this, SLOT(doResume()));
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSPausableThread::~CSPausableThread()
{
	delete controlMutex;
}

/*!
 * This function pauses our thread's event dispatch loop. Any event that is
 * already being processed will continue running, but we will pause the thread
 * before processing any more events.
 *
 * If the thread is already paused, then nothing happens.
 */
void CSPausableThread::pause()
{
	QMutexLocker locker(controlMutex);

	if(paused)
		return;

	paused = true;

	quit();
}

/*!
 * This function resumes the thread after it has been paused. If the thread is
 * still processing the same event it was processing when pause() was called,
 * then we schedule a restart to occur after the current event is done
 * processing and the thread is stopped. Any events that were in the event
 * queue before pause() was called will then be processed as per normal.
 *
 * If the thread isn't paused, or a resume has already been scheduled, then
 * nothing happens. If the thread is already totally stopped, we perform an
 * immediate resume, instead of scheduling the resume to occur when finished()
 * is emitted.
 */
void CSPausableThread::resume()
{
	QMutexLocker locker(controlMutex);

	if(!paused)
		return;

	if(resumeScheduled)
		return;

	if(isFinished())
	{
		start();

		paused = false;
		resumeScheduled = false;
	}
	else
	{
		resumeScheduled = true;
	}
}

/*!
 * This function is connected to our thread's finished() signal, and it resumes
 * the thread if a resume was scheduled previously. See resume() for more
 * information.
 */
void CSPausableThread::doResume()
{ /* SLOT */

	QMutexLocker locker(controlMutex);

	if(!resumeScheduled)
		return;

	paused = false;
	resumeScheduled = false;

	wait();
	start();
}
