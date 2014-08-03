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

#ifndef INCLUDE_LIBCUTE_THREAD_COLLECTION_THREAD_POOL_H
#define INCLUDE_LIBCUTE_THREAD_COLLECTION_THREAD_POOL_H

#include <QObject>

class QThread;
class QMutex;

class CSCollectionTypeResolver;
class CSAbstractCollection;
class CSPausableThread;
class CSCollectionJobExecutor;

/*!
 * \brief This class manages a pool of worker threads for our application.
 *
 * We deal with creating collections and moving them to an appropriate worker
 * thread, as well as gracefully stopping collection actions.
 */
class CSCollectionThreadPool : public QObject
{
	Q_OBJECT

	public:
		CSCollectionThreadPool(QObject *p = 0);
		virtual ~CSCollectionThreadPool();

		bool isInterruptible();

		void pause();
		void resume();
		bool stopGracefully();

	private:
		QMutex *controlMutex;
		bool paused;
		bool interruptible;

		CSPausableThread *thread;
		CSCollectionTypeResolver *resolver;
		CSCollectionJobExecutor *executor;

		void setInterruptible(bool i);

	public Q_SLOTS:
		void unserializeCollection(const QString &n,
			const QString &p, const QByteArray &d);
		void newCollection(const QString &n, const QString &p, bool s);
		void syncCollections(CSAbstractCollection *s,
			CSAbstractCollection *d);

	private Q_SLOTS:
		void doCollectionCreated(CSAbstractCollection *c);
		void doJobStarted(const QString &j, bool i);
		void doJobFinished(const QString &r);

	Q_SIGNALS:
		void interruptAllJobs();

		void jobStarted(const QString &, bool);
		void progressLimitsUpdated(int, int);
		void progressUpdated(int);
		void jobFinished(const QString &);

		void startUnserialize(const QString &,
			const QString &, const QByteArray &);
		void startNew(const QString &, const QString &, bool);
		void startSync(CSAbstractCollection *, CSAbstractCollection *);

		void collectionCreated(CSAbstractCollection *);
};

#endif
