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

#include "DirCollectionConfigWidget.h"

#include <QThread>
#include <QGridLayout>
#include <QGroupBox>
#include <QCheckBox>

/*!
 * This is our default constructor, which initializes our widget.
 *
 * \param p The GUI thread we should be a member of.
 */
CuteSyncDirCollectionConfigWidget::CuteSyncDirCollectionConfigWidget(QThread *t)
	: CuteSyncAbstractCollectionConfigWidget(t)
{
	layout = new QGridLayout(this);

	generalGroupBox = new QGroupBox(tr("General Options"), this);
	generalLayout = new QGridLayout(generalGroupBox);

	saveCheckBox = new QCheckBox(tr("Save collection information"), generalGroupBox);

	generalLayout->addWidget(saveCheckBox, 0, 0, 1, 1);
	generalGroupBox->setLayout(generalLayout);

	syncGroupBox = new QGroupBox(tr("Sync Options"), this);
	syncLayout = new QGridLayout(syncGroupBox);

	organizeCheckBox = new QCheckBox(tr("Automatic file organization"), syncGroupBox);

	syncLayout->addWidget(organizeCheckBox, 0, 0, 1, 1);
	syncGroupBox->setLayout(syncLayout);

	layout->addWidget(generalGroupBox, 0, 0, 1, 1);
	layout->addWidget(syncGroupBox, 1, 0, 1, 1);
	layout->setRowStretch(2, 1);
	setLayout(layout);
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CuteSyncDirCollectionConfigWidget::~CuteSyncDirCollectionConfigWidget()
{
}

/*!
 * This function applies our current state to our parent collection.
 */
void CuteSyncDirCollectionConfigWidget::apply()
{
	emit applyRequest();
}

/*!
 * This function resets our widget's options to the values currently set by our owning collection.
 */
void CuteSyncDirCollectionConfigWidget::reset()
{
	emit resetRequest();
}

/*!
 * This function retrieves our widget's current save state value.
 *
 * \return Our current save state value.
 */
bool CuteSyncDirCollectionConfigWidget::getSaveState() const
{
	switch(saveCheckBox->checkState())
	{
		case Qt::Unchecked:
			return false;

		case Qt::Checked:
			return true;

		default:
			return false;
	};
}

/*!
 * This function sets our widget's save state value to the given value.
 *
 * \param s True for save collection information, or false for discard.
 */
void CuteSyncDirCollectionConfigWidget::setSaveState(bool s)
{
	switch(s)
	{
		case true:
			saveCheckBox->setCheckState(Qt::Checked);
			break;

		case false:
			saveCheckBox->setCheckState(Qt::Unchecked);
			break;
	};
}

/*!
 * This function retrieves our widget's current auto-organize state value.
 *
 * \return Our current auto-organize state value.
 */
bool CuteSyncDirCollectionConfigWidget::getOrganizeState() const
{
	switch(organizeCheckBox->checkState())
	{
		case Qt::Unchecked:
			return false;

		case Qt::Checked:
			return true;

		default:
			return false;
	};
}

/*!
 * This function sets our widget's auto-organize state value to the given value.
 *
 * \param o True for automatically organize collection, or false for no organization.
 */
void CuteSyncDirCollectionConfigWidget::setOrganizeState(bool o)
{
	switch(o)
	{
		case true:
			organizeCheckBox->setCheckState(Qt::Checked);
			break;

		case false:
			organizeCheckBox->setCheckState(Qt::Unchecked);
			break;
	};
}
