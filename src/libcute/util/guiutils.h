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

#ifndef INCLUDE_CUTE_SYNC_UTIL_GUI_UTILS_H
#define INCLUDE_CUTE_SYNC_UTIL_GUI_UTILS_H

#include <QIcon>
#include <QString>

/*!
 * \brief This function provides various GUI-related utilities.
 */
class CSGUIUtils
{
	public:
		static QIcon getIconFromTheme(const QString &n);
};

#endif
