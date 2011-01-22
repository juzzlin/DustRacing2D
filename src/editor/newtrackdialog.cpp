// This file belongs to the "Dust" car racing game
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "newtrackdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>

NewTrackDialog::NewTrackDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Create a new track"));

    m_layout = new QGridLayout(this);

    m_okButton = new QPushButton(tr("Ok"));
    m_cancelButton = new QPushButton(tr("Cancel"));

    m_horSizeEdit = new QLineEdit;
    m_horSizeEdit->setValidator(new QIntValidator(this));

    m_verSizeEdit = new QLineEdit;
    m_verSizeEdit->setValidator(new QIntValidator(this));

    m_nameEdit = new QLineEdit;

    m_horSizeLabel = new QLabel(tr("Horizontal size:"));
    m_verSizeLabel = new QLabel(tr("Vertical size:"));
    m_nameLabel = new QLabel(tr("Track name:"));

    m_layout->addWidget(m_nameLabel, 0, 0);
    m_layout->addWidget(m_nameEdit, 0, 1);

    m_layout->addWidget(m_horSizeLabel, 1, 0);
    m_layout->addWidget(m_horSizeEdit, 1, 1);

    m_layout->addWidget(m_verSizeLabel, 2, 0);
    m_layout->addWidget(m_verSizeEdit, 2, 1);

    m_layout->addWidget(m_okButton, 3, 0);
    m_layout->addWidget(m_cancelButton, 3, 1);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

unsigned int NewTrackDialog::horSize() const
{
    return m_horSizeEdit->text().toInt();
}

unsigned int NewTrackDialog::verSize() const
{
    return m_verSizeEdit->text().toInt();
}

QString NewTrackDialog::name() const
{
    return m_nameEdit->text();
}
