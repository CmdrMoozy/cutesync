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

#include "CuteSyncInspectorConfigDialog.h"

#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QListView>

#include "widgets/CuteSyncDDSelectorModel.h"
#include "widgets/CuteSyncDDSelectorWidget.h"

/*!
 * This is our default constructor, which creates our new dialog.
 *
 * \param p Our parent widget.
 * \param f The window flags to use.
 */
CuteSyncInspectorConfigDialog::CuteSyncInspectorConfigDialog(QWidget *p, Qt::WindowFlags f)
	: QDialog(p, f)
{
	setWindowTitle("CuteSync - Inspector Configuration");
	createGUI();
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CuteSyncInspectorConfigDialog::~CuteSyncInspectorConfigDialog()
{
}

/*!
 * This function sets our display descriptor to the given value. This could, e.g., be used to update our dialog's
 * state with the state of the inspector we are configuring before displaying us.
 *
 * \param d The new display descriptor to use.
 */
void CuteSyncInspectorConfigDialog::setDisplayDescriptor(const CuteSyncAbstractCollection::DisplayDescriptor &d)
{
	displayDescriptor = d;
	updateDisplayDescriptor();
}

/*!
 * This function returns a pointer to the display descriptor we are currently working from.
 *
 * \return A pointer to our display descriptor.
 */
const CuteSyncAbstractCollection::DisplayDescriptor *CuteSyncInspectorConfigDialog::getDisplayDescriptor() const
{
	return &displayDescriptor;
}

/*!
 * We intercept show events to update our GUI before we are displayed.
 *
 * \param e The event we are handling.
 */
void CuteSyncInspectorConfigDialog::showEvent(QShowEvent *e)
{
	updateDisplayDescriptor();
	QDialog::showEvent(e);
}

/*!
 * This is a helper function that creates all of the widgets that make up our GUI.
 */
void CuteSyncInspectorConfigDialog::createGUI()
{
	layout = new QGridLayout(this);
	
	sortGroupBox = new QGroupBox(tr("Sort By Columns"), this);
	sortLayout = new QGridLayout(sortGroupBox);
	
	sortSelector = new CuteSyncDDSelectorWidget(sortGroupBox);
	sortModel = new CuteSyncDDSelectorModel(sortSelector);
	sortSelector->setModel(sortModel);
	
	ascendingButton = new QRadioButton(tr("Ascending"), sortGroupBox);
	descendingButton = new QRadioButton(tr("Descending"), sortGroupBox);
	
	sortLayout->addWidget( sortSelector,     0, 0, 1, 1 );
	sortLayout->addWidget( ascendingButton,  1, 0, 1, 1 );
	sortLayout->addWidget( descendingButton, 2, 0, 1, 1 );
	sortGroupBox->setLayout(sortLayout);
	
	displayGroupBox = new QGroupBox(tr("Display Columns"), this);
	displayLayout = new QGridLayout(displayGroupBox);
	
	displaySelector = new CuteSyncDDSelectorWidget(displayGroupBox);
	displayModel = new CuteSyncDDSelectorModel(displaySelector);
	displaySelector->setModel(displayModel);
	
	displayLayout->addWidget( displaySelector, 0, 0, 1, 1 );
	displayGroupBox->setLayout(displayLayout);
	
	buttonsWidget = new QWidget(this);
	buttonsLayout = new QGridLayout(buttonsWidget);
	
	okButton = new QPushButton(tr("Ok"), buttonsWidget);
	applyButton = new QPushButton(tr("Apply"), buttonsWidget);
	cancelButton = new QPushButton(tr("Cancel"), buttonsWidget);
	
	buttonsLayout->addWidget( okButton,     0, 1, 1, 1 );
	buttonsLayout->addWidget( applyButton,  0, 2, 1, 1 );
	buttonsLayout->addWidget( cancelButton, 0, 3, 1, 1 );
	buttonsLayout->setColumnStretch(0, 1);
	buttonsWidget->setLayout(buttonsLayout);
	
	layout->addWidget( sortGroupBox,    0, 0, 1, 1);
	layout->addWidget( displayGroupBox, 0, 1, 1, 1);
	layout->addWidget( buttonsWidget,   2, 0, 1, 2);
	layout->setRowStretch(1, 1);
	setLayout(layout);
	
	QObject::connect( okButton,     SIGNAL( clicked() ), this, SLOT( doOk()    ) );
	QObject::connect( applyButton,  SIGNAL( clicked() ), this, SLOT( doApply() ) );
	QObject::connect( cancelButton, SIGNAL( clicked() ), this, SLOT( close()   ) );
}

/*!
 * This is a helper function that updates our GUI to utilize current display descriptor information.
 * This should probably be called, e.g., when our dialog is about to be displayed.
 */
void CuteSyncInspectorConfigDialog::updateDisplayDescriptor() const
{
	sortModel->loadSortColumns(getDisplayDescriptor());
	displayModel->loadDisplayColumns(getDisplayDescriptor());
	
	if(getDisplayDescriptor()->s_order == Qt::AscendingOrder)
	{
		descendingButton->setChecked(false);
		ascendingButton->setChecked(true);
	}
	else
	{
		ascendingButton->setChecked(false);
		descendingButton->setChecked(true);
	}
}

/*!
 * This is a helper function that "applies" changes made to our display descriptor. This means that we
 * grab the new information from our widgets, and apply that information to our display descriptor
 * object, so the inspector we are configuring can use it. This should be called, e.g., when our "Apply"
 * or "OK" buttons are pushed.
 */
void CuteSyncInspectorConfigDialog::applyDisplayDescriptor()
{
	// Sort order.
	if(ascendingButton->isChecked())
		displayDescriptor.s_order = Qt::AscendingOrder;
	else
		displayDescriptor.s_order = Qt::DescendingOrder;
	
	// Sort columns.
	displayDescriptor.s_columns = sortModel->getColumnList();
	
	// Display columns.
	QList<CuteSyncAbstractCollection::Column> displayList = displayModel->getColumnList();
	displayDescriptor.columns.clear();
	for(int i = 0; i < displayModel->getColumnList().count(); ++i)
		displayDescriptor.columns.insert(i, displayList.at(i));
	
	// Update our GUI. :)
	updateDisplayDescriptor();
}

/*!
 * This function handles our "OK" button being clicked by applying our new descriptor settings, emitting an
 * accepted signal for our parent, and then closing the dialog.
 */
void CuteSyncInspectorConfigDialog::doOk()
{ /* SLOT */
	applyDisplayDescriptor();
	emit accepted();
	close();
}

/*!
 * This function handles our "Apply" button being clicked by applying our new descriptor settings and
 * emitting an accepted signal for our parent. Since this is just "Apply," we leave the dialog open.
 */
void CuteSyncInspectorConfigDialog::doApply()
{ /* SLOT */
	applyDisplayDescriptor();
	emit accepted();
}
