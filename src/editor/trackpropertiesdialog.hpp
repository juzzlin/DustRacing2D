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

#ifndef TRACKPROPERTIESDIALOG_HPP
#define TRACKPROPERTIESDIALOG_HPP

#include <QDialog>

class QGridLayout;
class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

/*! Dialog to set track properties */
class TrackPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor.
    TrackPropertiesDialog(QString name, unsigned int index, bool isUserTrack, QWidget * parent = nullptr);

    //! Get index.
    unsigned int index() const;

    //! Get name.
    QString name() const;

    //! Get user track property.
    bool isUserTrack() const;

private:
    QGridLayout * m_layout;
    QPushButton * m_okButton;
    QPushButton * m_cancelButton;
    QLineEdit * m_nameEdit;
    QLabel * m_nameLabel;
    QLineEdit * m_indexEdit;
    QLabel * m_indexLabel;
    QCheckBox * m_isUserTrackCheckBox;
    QLabel * m_isUserTrackLabel;
};

#endif // TRACKPROPERTIESDIALOG_HPP
