// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MWTEXT_H
#define MWTEXT_H

#include "mwwidget.h"
#include <QString>

//! Simple text label.
class MWText : public MWWidget
{
public:

    //! Constructor.
    explicit MWText(const QString & text, QSizeF size, MWWidget * parent = nullptr);

    //! Return the text
    QString text() const;

    //! Set the text
    void setText(const QString & text);

    //! Set the text color
    void setTextColor(QColor color);

    //! Get the text color
    QColor textColor() const;

protected:

    //! \reimp
    virtual void paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option, QWidget * widget);

private:

    QString m_text;
    QColor  m_textColor;

    Q_DISABLE_COPY(MWText)
};

#endif // MWTEXT_H
