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

#ifndef NEWTRACKDIALOG_H
#define NEWTRACKDIALOG_H

#include <QDialog>

class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class NewTrackDialog : public QDialog
{
    Q_OBJECT

public:

    explicit NewTrackDialog(QWidget *parent = 0);

    unsigned int horSize() const;

    unsigned int verSize() const;

    QString name() const;

signals:

public slots:

private:

    QGridLayout * m_layout;
    QPushButton * m_okButton;
    QPushButton * m_cancelButton;
    QLineEdit   * m_horSizeEdit;
    QLineEdit   * m_verSizeEdit;
    QLineEdit   * m_nameEdit;
    QLabel      * m_horSizeLabel;
    QLabel      * m_verSizeLabel;
    QLabel      * m_nameLabel;
};

#endif // NEWTRACKDIALOG_H
