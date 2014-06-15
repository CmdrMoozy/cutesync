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

#include "AbstractCollectionConfigWidget.h"

/*!
 * This is our default constructor, which essentially just propagates
 * constructor calls to our superclass.
 *
 * \param t The thread out widget should be a part of (i.e., the GUI thread).
 */
CuteSyncAbstractCollectionConfigWidget::
	CuteSyncAbstractCollectionConfigWidget()
	: QWidget(NULL, 0)
{
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CuteSyncAbstractCollectionConfigWidget::
	~CuteSyncAbstractCollectionConfigWidget()
{
}
