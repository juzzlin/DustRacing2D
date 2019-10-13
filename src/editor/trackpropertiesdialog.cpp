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

#include "trackpropertiesdialog.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

TrackPropertiesDialog::TrackPropertiesDialog(QString name, unsigned int index, bool isUserTrack, QWidget * parent)
  : QDialog(parent)
  , m_layout(new QGridLayout(this))
  , m_okButton(new QPushButton(tr("Ok")))
  , m_cancelButton(new QPushButton(tr("Cancel")))
  , m_nameEdit(new QLineEdit)
  , m_nameLabel(new QLabel(tr("Name:")))
  , m_indexEdit(new QLineEdit)
  , m_indexLabel(new QLabel(tr("Index:")))
  , m_isUserTrackCheckBox(new QCheckBox)
  , m_isUserTrackLabel(new QLabel(tr("User track:")))
{
    setWindowTitle(tr("Set track properties"));

    m_nameEdit->setText(name);
    QIntValidator * indexValidator = new QIntValidator(this);
    indexValidator->setRange(0, 999);
    m_indexEdit->setValidator(indexValidator);
    m_indexEdit->setText(QString("%1").arg(index));
    m_isUserTrackCheckBox->setChecked(isUserTrack);

    m_layout->addWidget(m_nameLabel, 0, 0);
    m_layout->addWidget(m_nameEdit, 0, 1);
    m_layout->addWidget(m_indexLabel, 1, 0);
    m_layout->addWidget(m_indexEdit, 1, 1);
    m_layout->addWidget(m_isUserTrackLabel, 2, 0);
    m_layout->addWidget(m_isUserTrackCheckBox, 2, 1);
    m_layout->addWidget(m_okButton, 3, 0);
    m_layout->addWidget(m_cancelButton, 3, 1);

    connect(m_okButton, &QPushButton::clicked, this, &TrackPropertiesDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &TrackPropertiesDialog::reject);
}

unsigned int TrackPropertiesDialog::index() const
{
    return m_indexEdit->text().toInt();
}

QString TrackPropertiesDialog::name() const
{
    return m_nameEdit->text();
}

bool TrackPropertiesDialog::isUserTrack() const
{
    return m_isUserTrackCheckBox->isChecked();
}
