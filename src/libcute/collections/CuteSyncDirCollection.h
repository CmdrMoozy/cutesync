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

#ifndef INCLUDE_CUTE_SYNC_DIR_COLLECTION_H
#define INCLUDE_CUTE_SYNC_DIR_COLLECTION_H

#include "libcute/collections/CuteSyncAbstractCollection.h"

#include <cstdint>

#include <QString>
#include <QHash>
#include <QList>
#include <QSet>

class QThread;

class CuteSyncAbstractCollectionConfigWidget;
class CuteSyncCollectionModel;

/*!
 * \brief This class implements a flat file collection.
 *
 * It is designed to load a collection of music from a flat directory, interpreting the files
 * literally.
 */
class CuteSyncDirCollection : public CuteSyncAbstractCollection
{
	Q_OBJECT

	public:
		CuteSyncDirCollection(CuteSyncCollectionModel *p = 0);
		CuteSyncDirCollection(const QString &n, CuteSyncCollectionModel *p = 0);
		CuteSyncDirCollection(const DisplayDescriptor *d, CuteSyncCollectionModel *p = 0);
		CuteSyncDirCollection(const QString &n, const DisplayDescriptor *d, CuteSyncCollectionModel *p = 0);
		virtual ~CuteSyncDirCollection();

		virtual QIcon getDisplayIcon() const;
		virtual QString getAboutText() const;

		virtual bool loadCollectionFromPath(const QString &p, bool f = true);
		virtual bool refresh();

		virtual QString getMountPoint() const;
		virtual QString getRelativePath(const QString &k) const;
		virtual QString getAbsolutePath(const QString &k) const;

		bool getRecursive() const;
		void setRecursive(bool r);

		bool isAutoOrganized() const;
		void setAutoOrganized(bool o);

		virtual bool flush();
		virtual void clear(bool f = true);

		virtual QByteArray serialize() const;
		virtual void unserialize(const QByteArray &d);

		virtual CuteSyncAbstractCollectionConfigWidget *getConfigurationWidget(QThread *t) const;

	protected:
		virtual bool quietDeleteTrack(const QString &k);
		virtual bool quietCopyTrack(const CuteSyncAbstractCollection *s, const QString &k);

	private:
		bool recursive, organize;
		QString root;

		QString filenameProcess(const QString &s) const;
		QString getAbsoluteWritePath(const CuteSyncAbstractCollection *s, const QString &k) const;

		void startJob(const QString &j);
		void finishJob();

	private slots:
		void doConfigurationApply();
		void doConfigurationReset();
};

#endif
