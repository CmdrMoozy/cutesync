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

#include "newcollectiondialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>

/*!
 * This is our default constructor, which initializes our dialog's GUI.
 *
 * \param p Our parent object.
 * \param f The window flags to use.
 */
CSNewCollectionDialog::CSNewCollectionDialog(
	QWidget *p, Qt::WindowFlags f)
	: QDialog(p, f)
{
	setModal(false);
	setWindowTitle(tr("CuteSync - Add Collection"));

	layout = new QGridLayout(this);

	nameLabel = new QLabel(tr("Name:"), this);
	nameLineEdit = new QLineEdit(this);

	pathLabel = new QLabel(tr("Path:"), this);
	pathLineEdit = new QLineEdit(this);
	browseButton = new QPushButton(tr("&Browse..."), this);

	saveCheckBox = new QCheckBox(tr("Save collection information"), this);

	buttonsWidget = new QWidget(this);
	buttonsLayout = new QGridLayout(buttonsWidget);

	doItButton = new QPushButton(tr("&Do It!"), buttonsWidget);
	cancelButton = new QPushButton(tr("&Cancel"), buttonsWidget);

	buttonsLayout->addWidget( doItButton,   0, 1, 1, 1 );
	buttonsLayout->addWidget( cancelButton, 0, 2, 1, 1 );
	buttonsLayout->setColumnStretch(0, 1);
	buttonsWidget->setLayout(buttonsLayout);

	layout->addWidget( nameLabel,     0, 0, 1, 1 );
	layout->addWidget( nameLineEdit,  0, 1, 1, 2 );
	layout->addWidget( pathLabel,     1, 0, 1, 1 );
	layout->addWidget( pathLineEdit,  1, 1, 1, 1 );
	layout->addWidget( browseButton,  1, 2, 1, 1 );
	layout->addWidget( saveCheckBox,  2, 0, 1, 3 );
	layout->addWidget( buttonsWidget, 4, 0, 1, 3 );
	layout->setRowStretch(3, 1);
	layout->setColumnStretch(1, 1);
	setLayout(layout);

	QObject::connect(browseButton, SIGNAL(clicked()),
		this, SLOT(doBrowse()));
	QObject::connect(doItButton, SIGNAL(clicked()),
		this, SLOT(doDoIt()));
	QObject::connect(cancelButton, SIGNAL(clicked()),
		this, SLOT(doCancel()));
}

/*!
 * This is our default destructor, which cleans up and destroys our dialog.
 */
CSNewCollectionDialog::~CSNewCollectionDialog()
{
}

/*!
 * This function retrieves the collection name the user has given us.
 *
 * \return The collection name.
 */
QString CSNewCollectionDialog::getName() const
{
	return nameLineEdit->text();
}

/*!
 * This function retrieves the collection path the user has given us.
 *
 * \return The collection path.
 */
QString CSNewCollectionDialog::getPath() const
{
	return pathLineEdit->text();
}

/*!
 * This function returns whether or not the user wants the collection's
 * information to be saved.
 *
 * \return Whether or not to save this collection.
 */
bool CSNewCollectionDialog::getSave() const
{
	return (saveCheckBox->checkState() == Qt::Checked);
}

/*!
 * We override QDialog's showEvent() method to reset our dialog.
 *
 * \param e The event we are handling.
 */
void CSNewCollectionDialog::showEvent(QShowEvent *e)
{
	reset();
	QDialog::showEvent(e);
}

/*!
 * This function resets our dialog to its default state.
 */
void CSNewCollectionDialog::reset()
{
	nameLineEdit->setText("");
	pathLineEdit->setText("");
	saveCheckBox->setCheckState(Qt::Unchecked);
}

/*!
 * This slot handles our browse button being clicked by popping up a directory
 * chooser dialog and then, if it is accepted, placing the resulting path in
 * our path line edit.
 */
void CSNewCollectionDialog::doBrowse()
{ /* SLOT */

	QString path = QFileDialog::getExistingDirectory(this,
		tr("Select Collection Path"), QDir::homePath());

	if(!path.isEmpty())
		pathLineEdit->setText(path);

}

/*!
 * This function handles our "Do It!" button being clicked by emitting an
 * accepted() signal and closing the dialog.
 */
void CSNewCollectionDialog::doDoIt()
{ /* SLOT */

	Q_EMIT accepted();
	close();

}

/*!
 * This function handles our cancel button being clicked by simply closing our
 * dialog.
 */
void CSNewCollectionDialog::doCancel()
{ /* SLOT */

	close();

}
