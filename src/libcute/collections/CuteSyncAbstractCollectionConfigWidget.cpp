#include "CuteSyncAbstractCollectionConfigWidget.h"

#include <QThread>

/*!
 * This is our default constructor, which essentially just propagates constructor calls to
 * our superclass.
 *
 * \param t The thread out widget should be a part of (i.e., the GUI thread).
 */
CuteSyncAbstractCollectionConfigWidget::CuteSyncAbstractCollectionConfigWidget(QThread *t)
	: QWidget(NULL, 0)
{
	moveToThread(t);
}

/*!
 * This is our default destructor, which cleans up and destroys our object.
 */
CuteSyncAbstractCollectionConfigWidget::~CuteSyncAbstractCollectionConfigWidget()
{
}
