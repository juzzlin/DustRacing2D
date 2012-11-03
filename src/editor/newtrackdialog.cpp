// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "newtrackdialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>

NewTrackDialog::NewTrackDialog(QWidget *parent) :
    QDialog(parent),
    m_layout(new QGridLayout(this)),
    m_okButton(new QPushButton(tr("Ok"))),
    m_cancelButton(new QPushButton(tr("Cancel"))),
    m_colsEdit(new QLineEdit),
    m_rowsEdit(new QLineEdit),
    m_nameEdit(new QLineEdit),
    m_colsLabel(new QLabel(tr("Number of tile columns:"))),
    m_rowsLabel(new QLabel(tr("Number of tile rows:"))),
    m_nameLabel(new QLabel(tr("Track name:")))
{
    setWindowTitle(tr("Create a new track"));

    m_colsEdit->setValidator(new QIntValidator(this));
    m_rowsEdit->setValidator(new QIntValidator(this));

    m_layout->addWidget(m_nameLabel, 0, 0);
    m_layout->addWidget(m_nameEdit, 0, 1);

    m_layout->addWidget(m_colsLabel, 1, 0);
    m_layout->addWidget(m_colsEdit, 1, 1);

    m_layout->addWidget(m_rowsLabel, 2, 0);
    m_layout->addWidget(m_rowsEdit, 2, 1);

    m_layout->addWidget(m_okButton, 3, 0);
    m_layout->addWidget(m_cancelButton, 3, 1);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

unsigned int NewTrackDialog::cols() const
{
    return m_colsEdit->text().toInt();
}

unsigned int NewTrackDialog::rows() const
{
    return m_rowsEdit->text().toInt();
}

QString NewTrackDialog::name() const
{
    return m_nameEdit->text();
}
