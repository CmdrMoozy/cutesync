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

#include "CuteSyncInspectorAboutDialog.h"

#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>

/*!
 * This is our default constructor, which creates our new dialog.
 *
 * \param p Our parent widget.
 * \param f The window flags to use.
 */
CuteSyncInspectorAboutDialog::CuteSyncInspectorAboutDialog(QWidget *p, Qt::WindowFlags f)
	: QDialog(p, f)
{
	setWindowTitle("CuteSync - Collection Information");
	
	layout = new QGridLayout(this);
	
	aboutText = new QTextEdit(this);
	aboutText->setReadOnly(true);
	aboutText->setWordWrapMode(QTextOption::NoWrap);
	
	closeButton = new QPushButton(tr("Clos&e"), this);
	
	layout->addWidget(aboutText, 0, 0, 1, 1);
	layout->addWidget(closeButton, 1, 0, 1, 1);
	layout->setRowStretch(0, 1);
	setLayout(layout);
	
	QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

/*!
 * This is our default destructor, which cleans up and destroys our dialog.
 */
CuteSyncInspectorAboutDialog::~CuteSyncInspectorAboutDialog()
{
}

/*!
 * We hook onto our superclass's show event to reset the GUI state when the dialog is displayed, making
 * it reusable.
 *
 * \param e The event we are handling.
 */
void CuteSyncInspectorAboutDialog::showEvent(QShowEvent *e)
{
	aboutText->moveCursor(QTextCursor::Start);
	QDialog::showEvent(e);
}

/*!
 * This function sets the about text we are supposed to be displaying, presumably from a collection.
 *
 * \param t The text to display.
 */
void CuteSyncInspectorAboutDialog::setAboutText(const QString &t)
{
	aboutText->setText(t);
	aboutText->moveCursor(QTextCursor::Start);
}
