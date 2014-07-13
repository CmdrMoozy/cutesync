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

#include "CuteSyncSyncDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

#include "libcute/collections/AbstractCollection.h"
#include "libcute/widgets/collectionmodel.h"

/*!
 * This is our default constructor, which creates a new sync dialog.
 *
 * \param c The collections we are working with.
 * \param p Our dialog's parent object.
 * \param f The window flags our dialog should use.
 */
CuteSyncSyncDialog::CuteSyncSyncDialog(CSCollectionModel *c, QWidget *p, Qt::WindowFlags f)
	: QDialog(p, f), collections(c)
{
	setWindowTitle("CuteSync - Synchronize Collections");

	createGUI();

	sourceComboBox->setModel(c);
	destinationComboBox->setModel(c);

	updateGUI();
}

/*!
 * This is our default destructor, which cleans up and destroys our dialog.
 */
CuteSyncSyncDialog::~CuteSyncSyncDialog()
{
}

/*!
 * We hook into our superclass's show event, in order to reset the GUI when this dialog is displayed, to
 * make it reusable.
 *
 * \param e The event we are handling.
 */
void CuteSyncSyncDialog::showEvent(QShowEvent *e)
{
	updateGUI();
	QDialog::showEvent(e);
}

/*!
 * This function sets up our initial GUI.
 */
void CuteSyncSyncDialog::createGUI()
{
	layout = new QGridLayout(this);

	sourceLabel = new QLabel(tr("Source:"), this);
	sourceComboBox = new QComboBox(this);

	destinationLabel = new QLabel(tr("Destination:"), this);
	destinationComboBox = new QComboBox(this);

	buttonsWidget = new QWidget(this);
	buttonsLayout = new QGridLayout(buttonsWidget);

	doItButton = new QPushButton(tr("Do It!"), buttonsWidget);
	cancelButton = new QPushButton(tr("Cancel"), buttonsWidget);

	buttonsLayout->addWidget( doItButton,   0, 1, 1, 1 );
	buttonsLayout->addWidget( cancelButton, 0, 2, 1, 1 );
	buttonsLayout->setColumnStretch(0, 1);
	buttonsWidget->setLayout(buttonsLayout);

	layout->addWidget( sourceLabel,         0, 0, 1, 1 );
	layout->addWidget( sourceComboBox,      0, 1, 1, 1 );
	layout->addWidget( destinationLabel,    1, 0, 1, 1 );
	layout->addWidget( destinationComboBox, 1, 1, 1, 1 );
	layout->addWidget( buttonsWidget,       3, 0, 1, 2 );
	layout->setColumnStretch(1, 1);
	layout->setRowStretch(2, 1);
	setLayout(layout);

	QObject::connect( doItButton,   SIGNAL( clicked() ), this, SLOT( doDoIt() ) );
	QObject::connect( cancelButton, SIGNAL( clicked() ), this, SLOT( close()  ) );
}

/*!
 * This function resets our GUI with our current data, e.g. when the collections available to us
 * changes.
 */
void CuteSyncSyncDialog::updateGUI()
{
	sourceComboBox->setCurrentIndex(-1);
	destinationComboBox->setCurrentIndex(-1);
}

/*!
 * This function handles the "do it" button by emititng an accepted signal so our parent can
 * start the actual synchronization.
 */
void CuteSyncSyncDialog::doDoIt()
{ /* SLOT */

	Q_EMIT accepted(collections->collectionAt(sourceComboBox->currentIndex()),
		collections->collectionAt(destinationComboBox->currentIndex()));

	close();

}
