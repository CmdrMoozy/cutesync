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

#include "settingsmanager.h"

#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QSettings>

#include "libcute/collections/abstractcollection.h"

// Load our default settings values into our static defaults list.

const QList< QPair<QString, QVariant> > CSSettingsManager::defaults
	= (QList< QPair<QString, QVariant> >())
		<< QPair<QString, QVariant>("display-descriptor",
			QVariant(CSAbstractCollection::serializeDisplayDescriptor(
				CSAbstractCollection::getDefaultDisplayDescriptor())))
		<< QPair<QString, QVariant>("saved-collections",
			QVariant(QList<QVariant>()))
		<< QPair<QString, QVariant>("window-geometry", QVariant(QByteArray()))
		<< QPair<QString, QVariant>("window-stat", QVariant(QByteArray()));

/*!
 * This is our default constructor, which creates a new settings manager
 * instance, and configures its default options and etc.
 */
CSSettingsManager::CSSettingsManager(QObject *p)
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
CSSettingsManager::~CSSettingsManager()
{
	delete sync;
	delete settings;
}

/*!
 * This function returns th enumber of settings keys this object is currently
 * keeping track of.
 *
 * \return The number of settings keys.
 */
int CSSettingsManager::count() const
{
	return settings->allKeys().count();
}

/*!
 * This function resets all of the settings in our settings manager back to
 * their default values.
 */
void CSSettingsManager::resetDefaults()
{
	QMutexLocker locker(sync);

	for(int i = 0; i < defaults.count(); ++i)
		setSetting(defaults.at(i).first, defaults.at(i).second);
}

/*!
 * This function resets the value of the given settings key back to its default
 * value. If the given key is invalid, then no action is taken.
 *
 * \param k The settings key to reset back to its default value.
 */
void CSSettingsManager::resetDefault(const QString &k)
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

/*!
 * This function sets a given setitng key to the given value.
 *
 * \param k The key whose value will be set.
 * \param v The new value for the given settings key.
 */
void CSSettingsManager::setSetting(const QString &k, const QVariant &v)
{
	QMutexLocker locker(sync);

	settings->setValue(k, v);
	Q_EMIT settingChanged(k, v);
}

/*!
 * This function returns whether or not our settings object currently contians
 * a value for the given settings key.
 *
 * \param k The key to search for in our settings object.
 * \return Whether or not our object contains a value for the given key.
 */
bool CSSettingsManager::containsSetting(const QString &k) const
{
	QMutexLocker locker(sync);

	return settings->contains(k);
}

/*!
 * This function returns the current value for the given settings key. If the
 * given key is invalid, then a default-constructed QVariant() is returned
 * instead.
 *
 * \param k The key to search for in our settings object.
 * \return The value associated with the given key.
 */
QVariant CSSettingsManager::getSetting(const QString &k) const
{
	QMutexLocker locker(sync);

	return settings->value(k, QVariant());
}

/*!
 * This function initializes our settings, ensuring that they are valid. Any
 * settings that are not set are given default values, and any settings that
 * are set to an invalid type are reset to their default value. Other settings
 * are left untouched.
 */
void CSSettingsManager::initializeDefaults()
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
