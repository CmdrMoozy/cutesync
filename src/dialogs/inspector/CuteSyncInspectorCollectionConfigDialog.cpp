#include "CuteSyncInspectorCollectionConfigDialog.h"

#include <QGridLayout>
#include <QPushButton>

#include "collections/CuteSyncAbstractCollectionConfigWidget.h"

/*!
 * This is our default constructor, which creates a new dialog. By default, we do not display a configuration widget,
 * so the dialog will only contain control buttons. Use setWidget() to give us something to actually configure.
 */
CuteSyncInspectorCollectionConfigDialog::CuteSyncInspectorCollectionConfigDialog(QWidget *p, Qt::WindowFlags f)
	: QDialog(p, f), widget(NULL)
{
	layout = new QGridLayout(this);
	
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
	
	layout->addWidget(buttonsWidget, 1, 0, 1, 1);
	layout->setRowStretch(0, 1);
	setLayout(layout);
	
	QObject::connect( okButton,     SIGNAL( clicked() ), this, SLOT( doOk()    ) );
	QObject::connect( applyButton,  SIGNAL( clicked() ), this, SLOT( doApply() ) );
	QObject::connect( cancelButton, SIGNAL( clicked() ), this, SLOT( close()   ) );
}

/*!
 * This is our default destructor, which cleans up and destroys our dialog.
 */
CuteSyncInspectorCollectionConfigDialog::~CuteSyncInspectorCollectionConfigDialog()
{
	// Delete the old widget, if applicable.
	
	if(widget != NULL)
	{
		delete widget;
		widget = NULL;
	}
}

/*!
 * This function allows our parent to change the configuration widget we should display. We just
 * disregard the old one, add the new one to our dialog and then reset it to a valid state.
 *
 * \param w The new widget to display.
 */
void CuteSyncInspectorCollectionConfigDialog::setWidget(CuteSyncAbstractCollectionConfigWidget *w)
{
	// Delete the old widget, if applicable.
	
	if(widget != NULL)
	{
		delete widget;
		widget = NULL;
	}
	
	// Set our new widget.
	
	widget = w;
	
	// Remove all widgets from our layout.
	
	while(layout->takeAt(0) != NULL);
	
	// Add the correct widgets back.
	
	if(widget != NULL)
	{
		widget->reset();
		layout->addWidget(widget, 0, 0, 1, 1);
	}
	
	layout->addWidget(buttonsWidget, 1, 0, 1, 1);
}

/*!
 * We intercept show events to update our GUI before we are displayed.
 *
 * \param e The event we are handling.
 */
void CuteSyncInspectorCollectionConfigDialog::showEvent(QShowEvent *e)
{
	if(widget != NULL)
		widget->reset();
	
	QDialog::showEvent(e);
}

/*!
 * We handle the "OK" button being pressed by telling our current widget (if any) to apply any changes
 * to its parent, and then we close the dialog.
 */
void CuteSyncInspectorCollectionConfigDialog::doOk()
{ /* SLOT */
	
	if(widget != NULL)
		widget->apply();
	
	close();
	
}

/*!
 * We handle the "Apply" button being pressed by telling our current widget (if any) to apply any changes
 * to its parent.
 */
void CuteSyncInspectorCollectionConfigDialog::doApply()
{ /* SLOT */
	
	if(widget != NULL)
		widget->apply();
	
}
