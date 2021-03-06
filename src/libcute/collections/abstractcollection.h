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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_ABSTRACT_COLLECTION_H
#define INCLUDE_LIBCUTE_COLLECTIONS_ABSTRACT_COLLECTION_H

#include <cstdint>

#include <QList>
#include <QString>
#include <QAbstractTableModel>
#include <QHash>
#include <QStringList>

class QMutex;

class CSCollectionModel;
class CSTrack;
class CSAbstractCollectionConfigWidget;
class CSGeneralCollectionConfigWidget;

/*!
 * \brief This class provides an interface for music collections.
 *
 * It is designed to ensure that subclasses provide functionality such that we
 * can take two arbitrary music collections - for example, a directory full of
 * music and an iPod, and then compare and sync them. This class tries to
 * provide as much general, common functionality as possible, but requires
 * subclasses to implement base functionality.
 *
 * Things that should notify of progress via our signals:
 *
 *     - bool loadCollectionFromPath(const QString &, bool)
 *     - bool syncFrom(const CSAbstractCollection *)
 *     - bool reload() [IF YOU OVERRIDE IT]
 *     - bool refresh() [IF YOU OVERRIDE IT]
 */
class CSAbstractCollection : public QAbstractTableModel
{
	Q_OBJECT

	/*
	 * Constructors / etc.:
	 */

	public:
		/*!
		 * This enumeration identifies a single column, independent of
		 * the "column index" in whatever widget is displaying us.
		 *
		 * NOTE THAT IF THIS ENUMERATION IS CHANGED OUR SETTINGS FILE
		 * FORMAT WILL ALSO BE CHANGED.
		 */
		enum Column
		{
			Artist      = 0,
			Album       = 1,
			Title       = 2,
			DiscNumber  = 3,
			TrackNumber = 4,
			TrackCount  = 5,
			Length      = 6,
			Year        = 7
		};

		/*!
		 * This structure stores: whether we should sort
		 * ascending/descending, what columns to sort by, and what
		 * display widget column index maps to what column.
		 *
		 * Note that the column order parameter, c, is encoded. That
		 * is, each column's index is stored in 4 bits, and they are
		 * arranged in order so the index occupying the low bits is the
		 * next-most important column for sorting.
		 *
		 * Note that if this structure is updated, then its related
		 * serialize/unserialize functions need to be updated to
		 * reflect that change as well.
		 */
		typedef struct DisplayDescriptor
		{
			Qt::SortOrder s_order;
			QList<CSAbstractCollection::Column> s_columns;
			QHash<int, CSAbstractCollection::Column> columns;
		} DisplayDescriptor;

		CSAbstractCollection(CSCollectionModel *p = 0);
		CSAbstractCollection(const QString &n,
			CSCollectionModel *p = 0);
		CSAbstractCollection(const DisplayDescriptor *d,
			CSCollectionModel *p = 0);
		CSAbstractCollection(const QString &n,
			const DisplayDescriptor *d,
			CSCollectionModel *p = 0);
		virtual ~CSAbstractCollection();

	/*
	 * Things you NEED TO IMPLEMENT if you are subclassing us:
	 */

	public:
		virtual QIcon getDisplayIcon() const = 0;
		virtual QString getAboutText() const = 0;

		virtual QString getMountPoint() const = 0;
		virtual QString getRelativePath(const QString &k) const = 0;
		virtual QString getAbsolutePath(const QString &k) const = 0;

		virtual bool flush() = 0;

		virtual QByteArray serialize() const = 0;
		virtual void unserialize(const QByteArray &d) = 0;

	public Q_SLOTS:
		virtual bool loadCollectionFromPath(const QString &p,
			bool f = true) = 0;

	protected:
		virtual bool quietDeleteTrack(const QString &k) = 0;
		virtual bool quietCopyTrack(const CSAbstractCollection *s,
			const QString &k) = 0;

	/*
	 * Things you CAN override, if you really want to:
	 */

	public:
		virtual CSAbstractCollectionConfigWidget *
			getConfigurationWidget() const;

		virtual QVariant getDisplayData(int r, int c) const;
		virtual QVariant getSortData(int r,
			CSAbstractCollection::Column c) const;

		void clear(bool f = true);

		virtual QString getName() const;
		virtual void setName(const QString &n);
		virtual bool isEnabled() const;
		virtual void setEnabled(bool e);
		virtual bool isSavedOnExit() const;

		virtual void sort() const;

		virtual bool reload();
		virtual bool refresh();

	public Q_SLOTS:
		virtual void setSaveOnExit(bool s);

		virtual bool deleteTracks(const QStringList &k);
		virtual bool copyTracks(const CSAbstractCollection *s,
			const QStringList &k);
		virtual bool syncFrom(CSAbstractCollection *o);

	/*
	 * Things you SHOULD NOT override:
	 */

	public:
		bool containsKey(const QString &k) const;
		int count() const;
		bool isEmpty() const;

		QList<QString> getKeysList() const;
		QList<QString> keysDifference(
			const CSAbstractCollection *o) const;

		bool isModified() const;

		const DisplayDescriptor *getDisplayDescriptor() const;
		void setDisplayDescriptor(const DisplayDescriptor *d);

		virtual int rowCount(
			const QModelIndex &p = QModelIndex()) const;
		virtual int columnCount(
			const QModelIndex &p = QModelIndex()) const;
		virtual QVariant data(const QModelIndex &i,
			int r = Qt::DisplayRole) const;
		virtual QVariant headerData(int s, Qt::Orientation o,
			int r = Qt::DisplayRole) const;

		bool isInterruptible() const;
		void setInterrupted(bool i);

	public Q_SLOTS:
		void setInterrupted();

	protected:
		QList<CSTrack *> allTracks() const;
		CSTrack *trackAt(int r) const;
		CSTrack *trackAt(const QString &k) const;
		void removeTrack(int r);
		void removeTrack(const QString &k);
		bool addTrack(CSTrack *t);

		bool isInterrupted() const;

		void setModified(bool m);

	private:
		void quicksort(int l, int r) const;
		int sortCmp(int ra, int rb) const;

		void setInterruptible(bool i);

	private Q_SLOTS:
		void doConfigurationApply();
		void doConfigurationReset();

		void doJobStarted(const QString &j, bool i);
		void doJobFinished(const QString &r);

	/*
	 * Static utility methods we provide. (Don't override these either.)
	 */

	public:
		static DisplayDescriptor getDefaultDisplayDescriptor();
		static QByteArray serializeDisplayDescriptor(
			const DisplayDescriptor &d);
		static DisplayDescriptor unserializeDisplayDescriptor(
			const QByteArray &d);

		static QString getColumnPrettyName(Column c);
		static QString getColumnName(Column c);

	/*
	 * Signals we define.
	 */

	Q_SIGNALS:
		void enabledChanged();
		void contentsChanged();

		void jobStarted(const QString &, bool);
		void progressLimitsUpdated(int, int);
		void progressUpdated(int);
		void jobFinished(const QString &);

	/*
	 * Member variables.
	 */

	private:
		QString name;
		bool modified;
		bool enabled;
		mutable QMutex *interruptibleMutex;
		bool interruptible;
		bool interrupted;
		bool saveOnExit;
		const DisplayDescriptor *displayDescriptor;
		mutable QList<CSTrack *> trackSort;
		QHash<QString, CSTrack *> trackHash;
};

#endif
