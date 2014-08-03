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

#ifndef INCLUDE_LIBCUTE_THREAD_COLLECTION_JOB_EXECUTOR_H
#define INCLUDE_LIBCUTE_THREAD_COLLECTION_JOB_EXECUTOR_H

#include <QObject>

class QThread;

class CSAbstractCollection;

/*!
 * \brief This is a simple class for executing jobs in a worker thread.
 *
 * To communicate from our GUI thread to our collections (which run in the
 * worker thread), we use the signals/slots mechanism. However, signals are not
 * designed to communicate with only ONE PARTICULAR object. This class provides
 * a single object that can be connected to signals, which then starts a
 * particular job on the particular object the signaler wanted to communicate
 * with.
 */
class CSCollectionJobExecutor : public QObject
{
	Q_OBJECT

	public:
		CSCollectionJobExecutor(QThread *t);
		virtual ~CSCollectionJobExecutor();

	public Q_SLOTS:
		void syncCollections(CSAbstractCollection *s,
			CSAbstractCollection *d);
};

#endif
