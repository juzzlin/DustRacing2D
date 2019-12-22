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

#include "targetnodesizedlg.hpp"
#include "tracktile.hpp"

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

TargetNodeSizeDlg::TargetNodeSizeDlg(QSizeF defaultSize, QWidget * parent)
  : QDialog(parent)
  , m_layout(new QGridLayout(this))
  , m_okButton(new QPushButton(tr("Ok")))
  , m_cancelButton(new QPushButton(tr("Cancel")))
  , m_widthEdit(new QLineEdit)
  , m_widthLabel(new QLabel(tr("Width:")))
  , m_heightEdit(new QLineEdit)
  , m_heightLabel(new QLabel(tr("Height:")))
{
    setWindowTitle(tr("Set target node size"));

    QIntValidator * widthValidator = new QIntValidator(this);
    widthValidator->setRange(TrackTile::width() / 2, TrackTile::width() * 4);
    m_widthEdit->setValidator(widthValidator);
    m_widthEdit->setText(QString("%1").arg(defaultSize.width()));

    QIntValidator * heightValidator = new QIntValidator(this);
    heightValidator->setRange(TrackTile::height() / 2, TrackTile::height() * 4);
    m_heightEdit->setValidator(widthValidator);
    m_heightEdit->setText(QString("%1").arg(defaultSize.height()));

    m_layout->addWidget(m_widthLabel, 0, 0);
    m_layout->addWidget(m_widthEdit, 0, 1);
    m_layout->addWidget(m_heightLabel, 1, 0);
    m_layout->addWidget(m_heightEdit, 1, 1);
    m_layout->addWidget(m_okButton, 3, 0);
    m_layout->addWidget(m_cancelButton, 3, 1);

    connect(m_okButton, &QPushButton::clicked, this, &TargetNodeSizeDlg::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &TargetNodeSizeDlg::reject);
}

QSizeF TargetNodeSizeDlg::targetNodeSize() const
{
    return QSizeF(m_widthEdit->text().toInt(), m_heightEdit->text().toInt());
}
