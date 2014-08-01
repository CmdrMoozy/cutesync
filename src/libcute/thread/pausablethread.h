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

#ifndef INCLUDE_LIBCUTE_THREAD_PAUSABLE_THREAD_H
#define INCLUDE_LIBCUTE_THREAD_PAUSABLE_THREAD_H

#include <QThread>

class QMutex;

/*!
 * \brief This class extends QThread by adding pause() and resume() functions.
 *
 * NOTE: This thread assumes that we are using the default implementation of
 * run(). Namely, we should be executing a standard event dispatch loop. If you
 * write your own run() function, then this class canot be used, and you will
 * need to implement your own pause/resume functionality.
 */
class CSPausableThread : public QThread
{
	Q_OBJECT

	public:
		CSPausableThread(QObject *p = 0);
		virtual ~CSPausableThread();

		void pause();
		void resume();

	private:
		QMutex *controlMutex;
		bool paused;
		bool resumeScheduled;

	private Q_SLOTS:
		void doResume();
};

#endif
