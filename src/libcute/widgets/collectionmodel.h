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

#ifndef INCLUDE_LIBCUTE_WIDGETS_COLLECTION_MODEL_H
#define INCLUDE_LIBCUTE_WIDGETS_COLLECTION_MODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QThread>

#include "libcute/collections/CollectionTypeResolver.h"

class QMutex;

class CuteSyncAbstractCollection;

/*!
 * \brief This class provides a storage container for a list of collections.
 *
 * It provides convenient QList-style access functions, as well as implementing
 * QAbstractListModel, so our items can very easily be displayed in a list
 * widget.
 */
class CSCollectionModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		CSCollectionModel(QObject *p = 0);
		virtual ~CSCollectionModel();

		virtual int rowCount(const QModelIndex &p =
			QModelIndex()) const;
		virtual QVariant data(const QModelIndex &i,
			int r = Qt::DisplayRole) const;
		virtual QVariant headerData(int s, Qt::Orientation o,
			int r = Qt::DisplayRole) const;

		void clear();
		int count() const;
		CuteSyncAbstractCollection *collectionAt(int i) const;
		CuteSyncAbstractCollection *collectionFromName(
			const QString &n) const;
		void removeCollectionAt(int i);
		void appendCollection(CuteSyncAbstractCollection *c);

		QList<QString> getCollectionNameList() const;

		bool isIdle() const;
		bool isInterruptAdvised() const;
		void setInterrupted(bool i);

		QList<QVariant> getSerializedList() const;

	public Q_SLOTS:
		void loadSerializedList(const QList<QVariant> &c);
		void newCollection(const QString &n, const QString &p, bool s);
		void reloadCollection(CuteSyncAbstractCollection *c);
		void refreshCollection(CuteSyncAbstractCollection *c);
		void syncCollections(CuteSyncAbstractCollection *s,
			CuteSyncAbstractCollection *d);
		// copy() delete()

	private:
		QMutex *mutex;
		CuteSyncCollectionTypeResolver resolver;
		QList<CuteSyncAbstractCollection *> itemList;
		CuteSyncAbstractCollection *currentJob;

	private Q_SLOTS:
		void doCollectionEnabledChanged();

		void doJobStarted(const QString &j);
		void doProgressLimitsUpdated(int min, int max);
		void doProgressUpdated(int p);
		void doJobFinished(const QString &r);

	Q_SIGNALS:
		void rowEnabledChanged(const QModelIndex &i);

		void jobStarted(const QString &);
		void progressLimitsUpdated(int, int);
		void progressUpdated(int);
		void jobFinished(const QString &);
};

#endif
