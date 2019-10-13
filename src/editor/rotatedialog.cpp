// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "rotatedialog.hpp"

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

RotateDialog::RotateDialog(QWidget * parent)
  : QDialog(parent)
  , m_layout(new QGridLayout(this))
  , m_okButton(new QPushButton(tr("Ok")))
  , m_cancelButton(new QPushButton(tr("Cancel")))
  , m_angleEdit(new QLineEdit)
  , m_angleLabel(new QLabel(tr("Angle in degrees:")))
{
    setWindowTitle(tr("Rotate Object"));

    QIntValidator * validator = new QIntValidator(this);
    validator->setRange(-360, 360);
    m_angleEdit->setValidator(validator);
    m_angleEdit->setText("90");

    m_layout->addWidget(m_angleLabel, 0, 0);
    m_layout->addWidget(m_angleEdit, 0, 1);
    m_layout->addWidget(m_okButton, 3, 0);
    m_layout->addWidget(m_cancelButton, 3, 1);

    connect(m_okButton, &QPushButton::clicked, this, &RotateDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &RotateDialog::reject);
}

int RotateDialog::angle() const
{
    return m_angleEdit->text().toInt();
}
