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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_DIR_COLLECTION_CONFIG_WIDGET_H
#define INCLUDE_LIBCUTE_COLLECTIONS_DIR_COLLECTION_CONFIG_WIDGET_H

#include "AbstractCollectionConfigWidget.h"

class QThread;
class QGridLayout;
class QGroupBox;
class QCheckBox;

/*!
 * \brief This class provides configuration specifically for DirCollections.
 *
 * We offer options above and beyond what general collection configuration
 * widgets do; namely sync options, e.g. allowing for our application to handle
 * file naming / directory structure for this collection.
 */
class CuteSyncDirCollectionConfigWidget :
	public CuteSyncAbstractCollectionConfigWidget
{
	Q_OBJECT

	public:
		CuteSyncDirCollectionConfigWidget(QThread *t);
		virtual ~CuteSyncDirCollectionConfigWidget();

		virtual void apply();
		virtual void reset();

		bool getSaveState() const;
		void setSaveState(bool s);

		bool getOrganizeState() const;
		void setOrganizeState(bool o);

	private:
		QGridLayout *layout;

		QGroupBox *generalGroupBox;
		QGridLayout *generalLayout;
		QCheckBox *saveCheckBox;

		QGroupBox *syncGroupBox;
		QGridLayout *syncLayout;
		QCheckBox *organizeCheckBox;

	Q_SIGNALS:
		void applyRequest();
		void resetRequest();
};

#endif
