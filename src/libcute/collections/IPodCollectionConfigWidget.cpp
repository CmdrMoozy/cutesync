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

#include "IPodCollectionConfigWidget.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QCheckBox>

/*!
 * This is our default constructor, which creates a new configuration widget.
 *
 * \param t The GUI thread for our widget.
 */
CuteSyncIPodCollectionConfigWidget::CuteSyncIPodCollectionConfigWidget()
	: CSAbstractCollectionConfigWidget()
{
	layout = new QGridLayout(this);

	generalGroupBox = new QGroupBox(tr("General Options"), this);
	generalLayout = new QGridLayout(generalGroupBox);

	saveCheckBox = new QCheckBox(tr("Save collection information"), generalGroupBox);

	generalLayout->addWidget(saveCheckBox, 0, 0, 1, 1);
	generalGroupBox->setLayout(generalLayout);

	syncGroupBox = new QGroupBox(tr("Collection Options"), this);
	syncLayout = new QGridLayout(syncGroupBox);

	albumArtworkCheckBox = new QCheckBox(tr("Enable album artwork"), syncGroupBox);

	caseSortCheckBox = new QCheckBox(tr("Case-insensitive track sorting"), syncGroupBox);

	ignoreCommonPrefixCheckBox = new QCheckBox(tr("Ignore common title prefixes for sorting ('The ...')"), syncGroupBox);

	syncLayout->addWidget(albumArtworkCheckBox, 0, 0, 1, 1);
	syncLayout->addWidget(caseSortCheckBox, 1, 0, 1, 1);
	syncLayout->addWidget(ignoreCommonPrefixCheckBox, 2, 0, 1, 1);
	syncGroupBox->setLayout(syncLayout);

	layout->addWidget(generalGroupBox, 0, 0, 1, 1);
	layout->addWidget(syncGroupBox, 1, 0, 1, 1);
	layout->setRowStretch(2, 1);
	setLayout(layout);
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CuteSyncIPodCollectionConfigWidget::~CuteSyncIPodCollectionConfigWidget()
{
}

/*!
 * This function applies our current state to our parent collection.
 */
void CuteSyncIPodCollectionConfigWidget::apply()
{
	Q_EMIT applyRequest();
}

/*!
 * This function resets our widget's options to the values currently set by our owning collection.
 */
void CuteSyncIPodCollectionConfigWidget::reset()
{
	Q_EMIT resetRequest();
}

/*!
 * This function retrieves our widget's current save state value.
 *
 * \return Our current save state value.
 */
bool CuteSyncIPodCollectionConfigWidget::getSaveState() const
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
void CuteSyncIPodCollectionConfigWidget::setSaveState(bool s)
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
 * This function retrieves our widget's current album artwork state value.
 *
 * \return Our current album artwork state value.
 */
bool CuteSyncIPodCollectionConfigWidget::getAlbumArtworkState() const
{
	switch(albumArtworkCheckBox->checkState())
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
 * This function sets our widget's album artwork state value to the given value.
 *
 * \param a True for enable album artwork, or false otherwise.
 */
void CuteSyncIPodCollectionConfigWidget::setAlbumArtworkState(bool a)
{
	switch(a)
	{
		case true:
			albumArtworkCheckBox->setCheckState(Qt::Checked);
			break;

		case false:
			albumArtworkCheckBox->setCheckState(Qt::Unchecked);
			break;
	};
}

/*!
 * This function retrieves our widget's current case-insensitive sorting state value.
 *
 * \return Our current case-insensitive sorting state value.
 */
bool CuteSyncIPodCollectionConfigWidget::getCaseSortState() const
{
	switch(caseSortCheckBox->checkState())
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
 * This function sets our widget's case-insensitive sorting state value to the given value.
 *
 * \param c True for case-insensitive sorting, or false otherwise.
 */
void CuteSyncIPodCollectionConfigWidget::setCaseSortState(bool c)
{
	switch(c)
	{
		case true:
			caseSortCheckBox->setCheckState(Qt::Checked);
			break;

		case false:
			caseSortCheckBox->setCheckState(Qt::Unchecked);
			break;
	};
}

/*!
 * This function retrieves our widget's current ignore common prefixes state value.
 *
 * \return Our current ignore common prefixes state value.
 */
bool CuteSyncIPodCollectionConfigWidget::getIgnoreCommonPrefixState() const
{
	switch(ignoreCommonPrefixCheckBox->checkState())
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
 * This function sets our widget's ignore common prefixes state value to the given value.
 *
 * \param i True for ignore common prefixes when sorting, or false otherwise.
 */
void CuteSyncIPodCollectionConfigWidget::setIgnoreCommonPrefixState(bool i)
{
	switch(i)
	{
		case true:
			ignoreCommonPrefixCheckBox->setCheckState(Qt::Checked);
			break;

		case false:
			ignoreCommonPrefixCheckBox->setCheckState(Qt::Unchecked);
			break;
	};
}
