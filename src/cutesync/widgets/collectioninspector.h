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

#ifndef INCLUDE_CUTE_SYNC_WIDGETS_COLLECTION_INSPECTOR_H
#define INCLUDE_CUTE_SYNC_WIDGETS_COLLECTION_INSPECTOR_H

#include <QWidget>

#include "libcute/collections/abstractcollection.h"

class QGridLayout;
class QPushButton;
class QTableView;
class QLabel;
class QProgressBar;
class QDialog;

class CSInspectorAboutDialog;
class CSInspectorCollectionConfigDialog;
class CSInspectorConfigDialog;
class CSSettingsManager;

/*!
 * \brief This class provides a widget to display info about a collection.
 *
 * It should be noted that this widget DOES NOT take ownership of any
 * collections you give it; that is, it's your responsibility to free them when
 * they aren't needed any more!
 */
class CSCollectionInspector : public QWidget
{
	Q_OBJECT

	public:
		CSCollectionInspector(CSSettingsManager *s,
			QWidget *p = 0);
		CSCollectionInspector(CSSettingsManager *s,
			CSAbstractCollection *c, QWidget *p = 0);
		virtual ~CSCollectionInspector();

		void clearCollection();

		CSAbstractCollection::DisplayDescriptor
			getDisplayDescriptor() const;
		CSAbstractCollection *getCollection() const;

	public Q_SLOTS:
		void setCollection(CSAbstractCollection *c);

	protected:
		virtual void timerEvent(QTimerEvent *e);

	private:
		CSSettingsManager *settingsManager;
		CSAbstractCollection::DisplayDescriptor displayDescriptor;

		CSInspectorAboutDialog *aboutDialog;
		CSInspectorCollectionConfigDialog *configDialog;
		CSInspectorConfigDialog *sortDialog;

		CSAbstractCollection *collection;

		QGridLayout *layout;

		QWidget *actionsList;
		QGridLayout *actionsLayout;
		QPushButton *aboutButton;
		QPushButton *refreshButton;
		QPushButton *configButton;
		QPushButton *sortButton;

		QTableView *collectionViewer;

		QLabel *spaceUsedLabel;
		QProgressBar *spaceUsedProgressBar;

		void createGUI();
		void createDialogs();
		void loadDisplayDescriptor();

	private Q_SLOTS:
		void doSortAccepted();

		void doAboutDevice();
		void doRefresh();
		void doConfig();
		void doSort();

		void doCollectionEnabledChanged();
		void doCollectionContentsChanged();

		void doSettingChanged(const QString &k, const QVariant &v);

	Q_SIGNALS:
		void reloadRequested(CSAbstractCollection *);
		void refreshRequested(CSAbstractCollection *);
};

#endif
