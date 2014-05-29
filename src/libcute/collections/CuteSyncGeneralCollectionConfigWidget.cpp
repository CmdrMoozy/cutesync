#include "CuteSyncGeneralCollectionConfigWidget.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QThread>

/*!
 * This is our default constructor, which initializes our widget.
 *
 * \param p The GUI thread we should be a member of.
 */
CuteSyncGeneralCollectionConfigWidget::CuteSyncGeneralCollectionConfigWidget(QThread *t)
	: CuteSyncAbstractCollectionConfigWidget(t)
{
	layout = new QGridLayout(this);
	
	groupBox = new QGroupBox(tr("General Options"), this);
	optionsLayout = new QGridLayout(groupBox);
	
	saveCheckBox = new QCheckBox(tr("Save collection information"), groupBox);
	
	optionsLayout->addWidget(saveCheckBox, 0, 0, 1, 1);
	optionsLayout->setRowStretch(1, 1);
	groupBox->setLayout(optionsLayout);
	
	layout->addWidget(groupBox, 0, 0, 1, 1);
	setLayout(layout);
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CuteSyncGeneralCollectionConfigWidget::~CuteSyncGeneralCollectionConfigWidget()
{
}

/*!
 * This function applies our current state to our parent collection.
 */
void CuteSyncGeneralCollectionConfigWidget::apply()
{
	emit applyRequest();
}

/*!
 * This function resets our widget's options to the values currently set by our owning collection.
 */
void CuteSyncGeneralCollectionConfigWidget::reset()
{
	emit resetRequest();
}

/*!
 * This function retrieves our widget's current save state value.
 *
 * \return Our current save state value.
 */
bool CuteSyncGeneralCollectionConfigWidget::getSaveState() const
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
void CuteSyncGeneralCollectionConfigWidget::setSaveState(bool s)
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
