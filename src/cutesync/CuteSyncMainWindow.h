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

#ifndef INCLUDE_CUTE_SYNC_MAIN_WINDOW_H
#define INCLUDE_CUTE_SYNC_MAIN_WINDOW_H

#include <QMainWindow>

#include "dialogs/CuteSyncNewCollectionDialog.h"

class QAction;
class QMenu;
class QToolBar;
class QGridLayout;
class QProgressBar;
class QLabel;
class QSplitter;
class QLocalServer;

class CuteSyncCollectionModel;
class CuteSyncCollectionListWidget;
class CuteSyncCollectionInspector;
class CuteSyncNewCollectionDialog;
class CuteSyncSyncDialog;
class CuteSyncAboutDialog;
class CuteSyncAbstractCollection;
class CuteSyncSettingsManager;

/*!
 * \brief This class defines the main window for our application.
 */
class CuteSyncMainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		CuteSyncMainWindow(CuteSyncSettingsManager *s,
			QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CuteSyncMainWindow();

	protected:
		virtual void closeEvent(QCloseEvent *e);

	private:
		QLocalServer *sappServer;

		CuteSyncSettingsManager *settingsManager;

		CuteSyncNewCollectionDialog *newCollectionDialog;
		CuteSyncSyncDialog *syncDialog;
		CuteSyncAboutDialog *aboutDialog;

		QAction *newCollectionAction;
		QAction *syncAction;
		QAction *removeCollectionAction;
		QAction *exitAction;
		QAction *resetSettingsAction;
		QAction *aboutCuteSyncAction;
		QAction *aboutQtAction;

		QMenu *fileMenu;
		QMenu *settingsMenu;
		QMenu *helpMenu;

		QToolBar *mainToolBar;

		QWidget *centralWidget;
		QGridLayout *layout;

		QSplitter *collectionsSplitter;
		CuteSyncCollectionListWidget *collectionsListWidget;
		CuteSyncCollectionModel *collectionsListModel;
		CuteSyncCollectionInspector *collectionInspector;

		QProgressBar *taskProgressBar;
		QLabel *taskLabel;

		void createActions();
		void createMenus();
		void createToolBars();
		void createDialogs();

	private Q_SLOTS:
		void doDuplicateInstanceDetected();

		void doNewCollectionAccepted();
		void doSyncAccepted(CuteSyncAbstractCollection *s,
			CuteSyncAbstractCollection *d);

		void doNewCollection();
		void doSync();
		void doRemoveCollection();
		void doResetSettings();
		void doAboutCuteSync();

		void doWorkerJobStarted(const QString &j);
		void doWorkerProgressLimitsUpdated(int min, int max);
		void doWorkerProgressUpdated(int p);
		void doWorkerJobFinished(const QString &r);

		void doSettingChanged(const QString &k, const QVariant &v);

	Q_SIGNALS:
		void startUnserialize(const QList<QVariant> &);
		void startNew(const QString &, const QString &, bool);
		void startReload(CuteSyncAbstractCollection *);
		void startRefresh(CuteSyncAbstractCollection *);
		void startSync(CuteSyncAbstractCollection *,
			CuteSyncAbstractCollection *);
		// copy(), delete()
};

#endif
