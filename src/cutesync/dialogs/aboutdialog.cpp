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

#include "aboutdialog.h"

#include <QGridLayout>
#include <QTabWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QIcon>

/*!
 * This constructor creates a new about dialog.
 *
 * \param p Our dialog's parent object.
 * \param f The window flags our dialog should use.
 */
CSAboutDialog::CSAboutDialog(QWidget *p, Qt::WindowFlags f)
	: QDialog(p, f)
{
	setWindowTitle("CuteSync - About");

	layout = new QGridLayout(this);

	aboutTabs = new QTabWidget(this);

	cuteSyncTab = new QWidget(aboutTabs);
	cuteSyncTabLayout = new QGridLayout(cuteSyncTab);

	logoLabel = new QLabel(tr("Icon"), cuteSyncTab);

	headerLabel = new QLabel(tr("CuteSync 1.0.0"), cuteSyncTab);
	headerLabel->setFont(QFont(headerLabel->font().family(), 18));
	headerLabel->setMargin(5);

	aboutText = new QTextEdit(cuteSyncTab);
	aboutText->setReadOnly(true);

	cuteSyncTabLayout->addWidget( logoLabel,   0, 0, 1, 1 );
	cuteSyncTabLayout->addWidget( headerLabel, 0, 1, 1, 1 );
	cuteSyncTabLayout->addWidget( aboutText,   1, 0, 1, 2 );
	cuteSyncTabLayout->setColumnStretch(1, 1);
	cuteSyncTabLayout->setRowStretch(1, 1);
	cuteSyncTab->setLayout(cuteSyncTabLayout);

	licenseTab = new QWidget(aboutTabs);
	licenseTabLayout = new QGridLayout(licenseTab);

	licenseText = new QTextEdit(licenseTab);
	licenseText->setLineWrapMode(QTextEdit::NoWrap);
	licenseText->setReadOnly(true);

	licenseTabLayout->addWidget( licenseText, 0, 0, 1, 1 );
	licenseTab->setLayout(licenseTabLayout);

	closeButton = new QPushButton(tr("Close"), this);

	aboutTabs->addTab(cuteSyncTab, tr("CuteSync"));
	aboutTabs->addTab(licenseTab, tr("License"));

	layout->addWidget(aboutTabs, 0, 0, 1, 1);
	layout->addWidget(closeButton, 1, 0, 1, 1);
	layout->setRowStretch(0, 1);
	setLayout(layout);

	QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	loadLicenseText();
}

/*!
 * This is our default destructor, which cleans up and destroys our dialog.
 */
CSAboutDialog::~CSAboutDialog()
{
}

/*!
 * This utility function loads the GPL license text from our resource package.
 */
void CSAboutDialog::loadLicenseText()
{
	QFile file(":/data/license.txt");
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream stream(&file);

		while(!stream.atEnd())
		{
			licenseText->append(stream.readLine());
		}

		file.close();
	}

	licenseText->moveCursor(QTextCursor::Start);
}
