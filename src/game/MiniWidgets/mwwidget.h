// This file is part of Dust Rallycross (DustRAC).
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

#ifndef MWWIDGET_H
#define MWWIDGET_H

#include <QGraphicsItem>
#include "mwobject.h"

class MWWidget : public MWObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    //! Constructor.
    //! \param size The size of the widget.
    //! \param parent The parent widget. If defined, then
    //!        the widget is by default positioned at the
    //!        center of the parent.
    explicit MWWidget(QSizeF size, MWWidget * parent = NULL);

    //! \reimp
    virtual QRectF boundingRect () const;

    //! \reimp
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    //! Set contents margins.
    void setContentsMargins(int left, int right, int top, int bottom);

    //! Get contents margins.
    void getContentsMargins(int & left, int & right, int & top, int & bottom) const;

    //! Set size
    void setSize(QSizeF size);

    //! Return size
    QSizeF size() const;

    //! Set background color
    void setBgColor(QColor color);

    //! Get background color
    QColor bgColor() const;

    //! Set foreground color
    void setFgColor(QColor color);

    //! Get foreground color
    QColor fgColor() const;

    //! Set widget clickable
    void setClickable(bool enable);

    //! Return true if widget is clickable
    bool clickable() const;

signals:

    //! Emitted, when the widget is clicked (and released)
    void clicked();

protected:

    virtual void paintBackground(QPainter * painter);
    virtual void adjustPos(MWWidget * parent);
    virtual void adjustSize(MWWidget * parent);

    //! \reimp
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);

    //! \reimp
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

private:

    MWWidget * m_parent;
    QSizeF     m_size;
    int        m_cm[4];
    QColor     m_bgColor;
    QColor     m_bgColor0;
    QColor     m_bgColorPressing;
    QColor     m_fgColor;
    bool       m_clickable;
    bool       m_pressing;

    Q_DISABLE_COPY(MWWidget)
};

#endif // MWWIDGET_H
