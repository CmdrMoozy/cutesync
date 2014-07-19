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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_ABSTRACT_COLLECTION_CONFIG_WIDGET_H
#define INCLUDE_LIBCUTE_COLLECTIONS_ABSTRACT_COLLECTION_CONFIG_WIDGET_H

#include <QWidget>

/*!
 * \brief This provides a superclass template for collection config widgets.
 *
 * Note that this superclass provides no configuration functionality - it
 * simply defines the methods our collection configuration dialog expects,
 * which need to be implemented by subclasses, and its constructor handles
 * moving us from the collection's thread to the GUI thread.
 *
 * It is expected that any subclasses will handle "generic" collection
 * configuration options, such as whether or not the collection should be saved
 * for next time our application is started, in addition to any collection-
 * subclass-specific options that need to be handled.
 */
class CSAbstractCollectionConfigWidget : public QWidget
{
	public:
		CSAbstractCollectionConfigWidget();
		virtual ~CSAbstractCollectionConfigWidget();

		virtual void apply() = 0;
		virtual void reset() = 0;
};

#endif
