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

#ifdef CUTESYNC_DEBUG
	#include "createipoddialog.h"

	#include <QGridLayout>
	#include <QLabel>
	#include <QLineEdit>
	#include <QPushButton>
	#include <QFileDialog>
	#include <QMessageBox>

	#include "libcute/collections/ipodcollection.h"

	/*!
	 * This is our default constructor, which creates a new "create iPod"
	 * dialog instance.
	 *
	 * \param p The dialog's parent widget.
	 * \param f The window flags the dialog should use.
	 */
	CSCreateIPodDialog::CSCreateIPodDialog(QWidget *p, Qt::WindowFlags f)
		: QDialog(p, f)
	{
		setWindowTitle("CuteSync - Create iPod");

		initializeGUI();
	}

	/*!
	 * This is our default destructor, which cleans up and destroys our
	 * dialog.
	 */
	CSCreateIPodDialog::~CSCreateIPodDialog()
	{
	}

	/*!
	 * We override our superclass's showEvent() function in order to clear
	 * our UI of any previously-entered values before being displayed.
	 *
	 * \param e The show event being handled.
	 */
	void CSCreateIPodDialog::showEvent(QShowEvent *e)
	{
		pathLineEdit->setText("");

		QDialog::showEvent(e);
	}

	/*!
	 * This is a small utility function which initializes our dialog's
	 * GUI contents.
	 */
	void CSCreateIPodDialog::initializeGUI()
	{
		layout = new QGridLayout(this);

		nameLabel = new QLabel(tr("Name:"), this);

		nameLineEdit = new QLineEdit(this);

		pathLabel = new QLabel(tr("Path:"), this);

		pathLineEdit = new QLineEdit(this);

		browseButton = new QPushButton(tr("&Browse..."), this);

		createButton = new QPushButton(tr("C&reate"), this);

		cancelButton = new QPushButton(tr("&Cancel"), this);

		layout->addWidget(nameLabel, 0, 0, 1, 1);
		layout->addWidget(nameLineEdit, 0, 1, 1, 3);
		layout->addWidget(pathLabel, 1, 0, 1, 1);
		layout->addWidget(pathLineEdit, 1, 1, 1, 2);
		layout->addWidget(browseButton, 1, 3, 1, 1);
		layout->addWidget(createButton, 3, 2, 1, 1);
		layout->addWidget(cancelButton, 3, 3, 1, 1);
		layout->setRowStretch(2, 1);
		layout->setColumnStretch(1, 1);
		setLayout(layout);

		QObject::connect(browseButton, SIGNAL(clicked(bool)),
			this, SLOT(doBrowse()));
		QObject::connect(createButton, SIGNAL(clicked(bool)),
			this, SLOT(doCreate()));
		QObject::connect(cancelButton, SIGNAL(clicked(bool)),
			this, SLOT(reject()));
	}

	/*!
	 * We handle our browse button being clicked by showing a file dialog
	 * which allows the user to select an existing directory to create an
	 * iPod structure in.
	 */
	void CSCreateIPodDialog::doBrowse()
	{ /* SLOT */

		QString path = QFileDialog::getExistingDirectory(this);

		if(path.length() > 0)
			pathLineEdit->setText(path);

	}

	/*!
	 * This function handles our create button being clicked by attemping
	 * to create a new iPod directory structure, and (if successful)
	 * notifying any listeners that this has occurred.
	 */
	void CSCreateIPodDialog::doCreate()
	{ /* SLOT */

		bool r = CSIPodCollection::createFalseIPod(
			nameLineEdit->text(), pathLineEdit->text());

		if(!r)
		{
			QMessageBox::critical(this, tr("Creating iPod Failed"),
				tr("Failed to create false iPod structure."));

			return;
		}

		Q_EMIT ipodCreated(nameLineEdit->text(), pathLineEdit->text());

		accept();

	}
#endif
