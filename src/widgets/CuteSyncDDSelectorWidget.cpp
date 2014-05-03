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

#include "CuteSyncDDSelectorWidget.h"

#include <QGridLayout>
#include <QListView>
#include <QPushButton>

#include "widgets/CuteSyncDDSelectorModel.h"

/*!
 * This is our default constructor, which creates our new widget.
 *
 * \param p Our parent widget.
 */
CuteSyncDDSelectorWidget::CuteSyncDDSelectorWidget(QWidget *p)
	: QWidget(p)
{
	layout = new QGridLayout(this);
	
	view = new QListView(this);
	
	upButton = new QPushButton(tr("Move Up"), this);
	downButton = new QPushButton(tr("Move Down"), this);
	
	layout->addWidget(view, 0, 0, 1, 2);
	layout->addWidget(upButton, 1, 0, 1, 1);
	layout->addWidget(downButton, 1, 1, 1, 1);
	layout->setRowStretch(0, 1);
	setLayout(layout);
	
	QObject::connect( upButton,   SIGNAL( clicked() ), this, SLOT( doUpClicked()   ) );
	QObject::connect( downButton, SIGNAL( clicked() ), this, SLOT( doDownClicked() ) );
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CuteSyncDDSelectorWidget::~CuteSyncDDSelectorWidget()
{
}

/*!
 * This function sets the model our list view will use. Note that we don't take ownership of the model; if it
 * needs to be deleted, then YOU should delete it.
 *
 * \param m The new model to use.
 */
void CuteSyncDDSelectorWidget::setModel(CuteSyncDDSelectorModel *m)
{
	view->setModel(m);
}

/*!
 * This function returns a pointer to the model our list view is currently using.
 *
 * \return The model we are currently using.
 */
CuteSyncDDSelectorModel *CuteSyncDDSelectorWidget::model() const
{
	return dynamic_cast<CuteSyncDDSelectorModel *>(view->model());
}

/*!
 * This function handles our up button being clicked by alerting our model that it needs to move the item
 * up, and by updating the selection in our view to match.
 */
void CuteSyncDDSelectorWidget::doUpClicked()
{
	CuteSyncDDSelectorModel *m = model();
	
	if( (m != NULL) && (view->selectionModel()->hasSelection()) )
	{
		m->moveUp(view->selectionModel()->selectedIndexes().front().row());
		view->setCurrentIndex(m->index(view->selectionModel()->selectedIndexes().front().row() - 1, 0));
	}
}

/*!
 * This function handles our down button being clicked by alerting our model that it needs to move the item
 * down, and by updating the selection in our view to match.
 */
void CuteSyncDDSelectorWidget::doDownClicked()
{
	CuteSyncDDSelectorModel *m = model();
	
	if( (m != NULL) && (view->selectionModel()->hasSelection()) )
	{
		m->moveDown(view->selectionModel()->selectedIndexes().front().row());
		view->setCurrentIndex(m->index(view->selectionModel()->selectedIndexes().front().row() + 1, 0));
	}
}
