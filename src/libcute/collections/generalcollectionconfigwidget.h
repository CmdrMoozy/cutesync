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

#ifndef INCLUDE_LIBCUTE_COLLECTIONS_GENERAL_COLLECTION_CONFIG_WIDGET_H
#define INCLUDE_LIBCUTE_COLLECTIONS_GENERAL_COLLECTION_CONFIG_WIDGET_H

#include "abstractcollectionconfigwidget.h"

class QGridLayout;
class QGroupBox;
class QCheckBox;

class CSGeneralCollectionConfigWidget :
	public CSAbstractCollectionConfigWidget
{
	Q_OBJECT

	public:
		CSGeneralCollectionConfigWidget();
		virtual ~CSGeneralCollectionConfigWidget();

		virtual void apply();
		virtual void reset();

		bool getSaveState() const;
		void setSaveState(bool s);

	private:
		QGridLayout *layout;
		QGroupBox *groupBox;
		QGridLayout *optionsLayout;
		QCheckBox *saveCheckBox;

	Q_SIGNALS:
		void applyRequest();
		void resetRequest();
};

#endif
