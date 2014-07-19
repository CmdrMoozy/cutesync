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

class QAction;
class QMenu;
class QToolBar;
class QGridLayout;
class QProgressBar;
class QLabel;
class QSplitter;
class QLocalServer;

class CSMainMenuBar;
class CSCollectionModel;
class CSCollectionListWidget;
class CSCollectionInspector;
class CSNewCollectionDialog;
class CSSyncDialog;
class CSAbstractCollection;
class CSSettingsManager;

/*!
 * \brief This class defines the main window for our application.
 */
class CSMainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		CSMainWindow(QWidget *p = 0, Qt::WindowFlags f = 0);
		virtual ~CSMainWindow();

	protected:
		virtual void closeEvent(QCloseEvent *e);

	private:
		CSSettingsManager *settingsManager;

		CSNewCollectionDialog *newCollectionDialog;
		CSSyncDialog *syncDialog;

		CSMainMenuBar *mainMenuBar;

		QWidget *centralWidget;
		QGridLayout *layout;

		QSplitter *collectionsSplitter;
		CSCollectionListWidget *collectionsListWidget;
		CSCollectionModel *collectionsListModel;
		CSCollectionInspector *collectionInspector;

		QProgressBar *taskProgressBar;
		QLabel *taskLabel;

		void createMenus();
		void createDialogs();

	private Q_SLOTS:
		void doNewCollectionAccepted();
		void doSyncAccepted(CSAbstractCollection *s,
			CSAbstractCollection *d);

		void doNewCollection();
		void doSync();
		void doRemoveCollection();
		void doResetSettings();

		void doWorkerJobStarted(const QString &j);
		void doWorkerProgressLimitsUpdated(int min, int max);
		void doWorkerProgressUpdated(int p);
		void doWorkerJobFinished(const QString &r);

		void doSettingChanged(const QString &k, const QVariant &v);

	Q_SIGNALS:
		void startUnserialize(const QList<QVariant> &);
		void startNew(const QString &, const QString &, bool);
		void startReload(CSAbstractCollection *);
		void startRefresh(CSAbstractCollection *);
		void startSync(CSAbstractCollection *,
			CSAbstractCollection *);
		// copy(), delete()
};

#endif
