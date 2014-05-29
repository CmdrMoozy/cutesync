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

#ifndef INCLUDE_CUTE_SYNC_SETTINGS_MANAGER_H
#define INCLUDE_CUTE_SYNC_SETTINGS_MANAGER_H

#include <QObject>
#include <QList>
#include <QPair>

class QMutex;
class QString;
class QSettings;

/*!
 * \brief This class provides a settings manager for our application.
 *
 * It implements some functionality on top of that provided by RSettingsManager, and it provides
 * a set of default options specific to our application.
 */
class CuteSyncSettingsManager : public QObject
{
	Q_OBJECT

	public:
		CuteSyncSettingsManager(QObject *p = 0);
		virtual ~CuteSyncSettingsManager();

		int count() const;

		void resetDefaults();
		void resetDefault(const QString &k);

		void setSetting(const QString &k, const QVariant &v);
		bool containsSetting(const QString &k) const;
		QVariant getSetting(const QString &k) const;

	private:
		static const QList< QPair<QString, QVariant> > defaults;
		QSettings *settings;
		QMutex *sync;

		void initializeDefaults();

	signals:
		void settingChanged(const QString &, const QVariant &);
};

#endif
