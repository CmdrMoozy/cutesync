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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_IPOD_COLLECTION_H
#define INCLUDE_LIBCUTE_COLLECTIONS_IPOD_COLLECTION_H

#include "libcute/collections/AbstractCollection.h"

#include <QString>
#include <QList>
#include <QHash>

extern "C" {
	#include <glib.h>
	#include <gpod-1.0/gpod/itdb.h>
}

class QThread;

class CuteSyncAbstractCollectionConfigWidget;
class CuteSyncCollectionModel;

/*!
 * \brief This class implements an iPod collection.
 *
 * It is designed to load an iTunes DB stored on an iPod device via libgpod and
 * parse the information into a usable format.
 */
class CuteSyncIPodCollection : public CuteSyncAbstractCollection
{
	Q_OBJECT

	public:
		CuteSyncIPodCollection(CuteSyncCollectionModel *p = 0);
		CuteSyncIPodCollection(const QString &n,
			CuteSyncCollectionModel *p = 0);
		CuteSyncIPodCollection(const DisplayDescriptor *d,
			CuteSyncCollectionModel *p = 0);
		CuteSyncIPodCollection(const QString &n,
			const DisplayDescriptor *d,
			CuteSyncCollectionModel *p = 0);
		virtual ~CuteSyncIPodCollection();

		virtual QIcon getDisplayIcon() const;
		virtual QString getAboutText() const;

		virtual bool loadCollectionFromPath(const QString &p,
			bool f = true);

		virtual QString getMountPoint() const;
		virtual QString getRelativePath(const QString &k) const;
		virtual QString getAbsolutePath(const QString &k) const;

		bool getAlbumArtworkEnabled() const;
		void setAlbumArtworkEnabled(bool a);

		bool getCaseInsensitiveSort() const;
		void setCaseInsensitiveSort(bool c);

		bool getIgnoreCommonPrefixes() const;
		void setIgnoreCommonPrefixes(bool i);

		virtual bool flush();
		virtual void clear(bool f = true);

		virtual QByteArray serialize() const;
		virtual void unserialize(const QByteArray &d);

		virtual CuteSyncAbstractCollectionConfigWidget *
			getConfigurationWidget(QThread *t) const;

	protected:
		virtual bool quietDeleteTrack(const QString &k);
		virtual bool quietCopyTrack(
			const CuteSyncAbstractCollection *s, const QString &k);

	private:
		bool optionsModified, artwork, caselessSort, ignorePrefixes;
		Itdb_iTunesDB *itdb;
		bool itdbModified;
		QString root;

		gpointer getTrackCoverArt(const CuteSyncAbstractCollection *s,
			const QString &k);

		void refreshCollectionOptions();

	private slots:
		void doConfigurationApply();
		void doConfigurationReset();
};

#endif
