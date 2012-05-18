// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "trackpropertiesdialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QIntValidator>

TrackPropertiesDialog::TrackPropertiesDialog(unsigned int lapCount, QWidget * parent)
: QDialog(parent)
, m_layout(new QGridLayout(this))
, m_okButton(new QPushButton(tr("Ok")))
, m_cancelButton(new QPushButton(tr("Cancel")))
, m_lapCountEdit(new QLineEdit)
, m_lapCountLabel(new QLabel(tr("Number of laps:")))
{
    setWindowTitle(tr("Set track properties"));

    QIntValidator * validator = new QIntValidator(this);
    validator->setRange(1, 99);
    m_lapCountEdit->setValidator(validator);
    m_lapCountEdit->setText(QString("%1").arg(lapCount));

    m_layout->addWidget(m_lapCountLabel, 0, 0);
    m_layout->addWidget(m_lapCountEdit, 0, 1);

    m_layout->addWidget(m_okButton, 3, 0);
    m_layout->addWidget(m_cancelButton, 3, 1);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

unsigned int TrackPropertiesDialog::lapCount() const
{
    return m_lapCountEdit->text().toInt();
}
