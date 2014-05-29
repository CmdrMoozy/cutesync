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

#include "CuteSyncSettingsManager.h"

#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QSettings>

#include "libcute/collections/CuteSyncAbstractCollection.h"

// Load our default settings values into our static defaults list.

const QList< QPair<QString, QVariant> > CuteSyncSettingsManager::defaults
	= (QList< QPair<QString, QVariant> >())
		<< QPair<QString, QVariant>("display-descriptor",
			QVariant(CuteSyncAbstractCollection::serializeDisplayDescriptor(
				CuteSyncAbstractCollection::getDefaultDisplayDescriptor())))
		<< QPair<QString, QVariant>("saved-collections",
			QVariant(QList<QVariant>()))
		<< QPair<QString, QVariant>("window-geometry", QVariant(QByteArray()))
		<< QPair<QString, QVariant>("window-stat", QVariant(QByteArray()));

/*!
 * This is our default constructor, which creates a new settings manager instance, and configures its
 * default options and etc.
 */
CuteSyncSettingsManager::CuteSyncSettingsManager(QObject *p)
	: QObject(p)
{
	sync = new QMutex(QMutex::Recursive);

	settings = new QSettings(QSettings::UserScope,
		"CuteSync", "CuteSync", this);

	initializeDefaults();
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CuteSyncSettingsManager::~CuteSyncSettingsManager()
{
	delete sync;
	delete settings;
}

int CuteSyncSettingsManager::count() const
{
	return settings->allKeys().count();
}

void CuteSyncSettingsManager::resetDefaults()
{
	QMutexLocker locker(sync);

	for(int i = 0; i < defaults.count(); ++i)
		setSetting(defaults.at(i).first, defaults.at(i).second);
}

void CuteSyncSettingsManager::resetDefault(const QString &k)
{
	QMutexLocker locker(sync);

	for(int i = 0; i < defaults.count(); ++i)
	{
		if(defaults.at(i).first == k)
		{
			setSetting(k, defaults.at(i).second);
			break;
		}
	}
}

void CuteSyncSettingsManager::setSetting(const QString &k, const QVariant &v)
{
	QMutexLocker locker(sync);

	settings->setValue(k, v);
	emit settingChanged(k, v);
}

bool CuteSyncSettingsManager::containsSetting(const QString &k) const
{
	QMutexLocker locker(sync);

	return settings->contains(k);
}

QVariant CuteSyncSettingsManager::getSetting(const QString &k) const
{
	QMutexLocker locker(sync);

	return settings->value(k, QVariant());
}

/*!
 * This function initializes our settings, ensuring that they are valid. Any settings that are not set are
 * given default values, and any settings that are set to an invalid type are reset to their default value.
 * Other settings are left untouched.
 */
void CuteSyncSettingsManager::initializeDefaults()
{
	QMutexLocker locker(sync);

	for(int i = 0; i < defaults.count(); ++i)
	{
		if(!containsSetting(defaults.at(i).first))
		{
			setSetting(defaults.at(i).first,
				defaults.at(i).second);
		}
	}
}
