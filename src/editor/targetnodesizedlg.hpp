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

#ifndef TARGETNODESIZEDLG_HPP
#define TARGETNODESIZEDLG_HPP

#include <QDialog>

class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class TargetNodeSizeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TargetNodeSizeDlg(QSizeF defaultSize, QWidget * parent = 0);

    QSizeF targetNodeSize() const;

private:
    QGridLayout * m_layout;
    QPushButton * m_okButton;
    QPushButton * m_cancelButton;
    QLineEdit * m_widthEdit;
    QLabel * m_widthLabel;
    QLineEdit * m_heightEdit;
    QLabel * m_heightLabel;
};

#endif // TARGETNODESIZEDLG_HPP
