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

#include "guiutils.h"

#include <QFile>
#include <QDebug>

/*!
 * This function will return the icon for the given standard name. We try to
 * use an icon from the operating system's icon theme, but failing that, we
 * try to use a fallback from our internal icon store.
 *
 * If no icon can be found, a null QIcon is returned instead.
 *
 * The default names supported by QIcon (and, therefore, this function) can be
 * found here:
 *
 *     http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 *
 * \param n The name of the icon to find.
 * \return An icon object corresponding to the given name.
 */
QIcon CSGUIUtils::getIconFromTheme(const QString &n)
{
	QIcon defaultIcon;

	QString defaultPath = n;
	defaultPath.prepend(":/icons/");
	defaultPath.append(".png");

	if(QFile::exists(defaultPath))
		defaultIcon = QIcon(defaultPath);
	else
		qDebug("Couldn't find icon: %s", qPrintable(n));

	return QIcon::fromTheme(n, defaultIcon);
}
