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

#include "libcute/collections/collectiontyperesolver.h"

class CSAbstractCollection;
class CSCollectionThreadPool;
class CSCollectionListItem;

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
		CSAbstractCollection *collectionAt(int i) const;
		CSAbstractCollection *collectionFromName(
			const QString &n) const;
		void removeCollectionAt(int i);
		void appendCollection(CSAbstractCollection *c);

		QList<QString> getCollectionNameList() const;

		QList<QVariant> getSerializedList() const;

		bool stopGracefully();

	public Q_SLOTS:
		void loadSerializedList(const QList<QVariant> &c);
		void newCollection(const QString &n, const QString &p, bool s);
		void reloadCollection(CSAbstractCollection *c);
		void refreshCollection(CSAbstractCollection *c);
		void syncCollections(CSAbstractCollection *s,
			CSAbstractCollection *d);
		// copy() delete()

	private:
		CSCollectionThreadPool *threadPool;
		QList<CSCollectionListItem *> itemList;

		CSCollectionListItem *itemForCollection(
			const CSAbstractCollection *c);

		bool alreadyContainsName(const QString &n) const;

	private Q_SLOTS:
		void doCollectionEnabledChanged();

		void doCollectionCreated(CSAbstractCollection *);

	Q_SIGNALS:
		void rowEnabledChanged(const QModelIndex &i);

		void startUnserialize(const QString &, const QString &,
			const QByteArray &);
		void startNew(const QString &, const QString &, bool);

		void jobStarted(const QString &, bool);
		void progressLimitsUpdated(int, int);
		void progressUpdated(int);
		void jobFinished(const QString &);
};

#endif
