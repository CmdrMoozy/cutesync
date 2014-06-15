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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_IPOD_COLLECTION_CONFIG_WIDGET_H
#define INCLUDE_LIBCUTE_COLLECTIONS_IPOD_COLLECTION_CONFIG_WIDGET_H

#include "AbstractCollectionConfigWidget.h"

class QGridLayout;
class QGroupBox;
class QCheckBox;

/*!
 * \brief This class provides configuration specifically for IPodCollections.
 *
 * We offer some advanced options specifically for the iPod, e.g. allowing
 * advanced track sorting (case-insensitivity, ignoring "The" in titles, etc.).
 */
class CuteSyncIPodCollectionConfigWidget :
	public CuteSyncAbstractCollectionConfigWidget
{
	Q_OBJECT

	public:
		CuteSyncIPodCollectionConfigWidget();
		virtual ~CuteSyncIPodCollectionConfigWidget();

		virtual void apply();
		virtual void reset();

		bool getSaveState() const;
		void setSaveState(bool s);

		bool getAlbumArtworkState() const;
		void setAlbumArtworkState(bool a);

		bool getCaseSortState() const;
		void setCaseSortState(bool c);

		bool getIgnoreCommonPrefixState() const;
		void setIgnoreCommonPrefixState(bool i);

	private:
		QGridLayout *layout;

		QGroupBox *generalGroupBox;
		QGridLayout *generalLayout;
		QCheckBox *saveCheckBox;

		QGroupBox *syncGroupBox;
		QGridLayout *syncLayout;
		QCheckBox *albumArtworkCheckBox;
		QCheckBox *caseSortCheckBox;
		QCheckBox *ignoreCommonPrefixCheckBox;

	Q_SIGNALS:
		void applyRequest();
		void resetRequest();
};

#endif
