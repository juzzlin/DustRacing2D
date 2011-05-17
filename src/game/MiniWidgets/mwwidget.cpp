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

#include <QPainter>
#include "mwwidget.h"

MWWidget::MWWidget(QSizeF size, MWWidget * parent) :
    m_parent(parent),
    m_size(size),
    m_cm({1, 1, 1, 1}),
    m_bgColor(QColor(0, 0, 0)),
    m_fgColor(QColor(255, 255, 255)),
    m_clickable(false),
    m_pressing(false)
{
    adjustPos(parent);
    adjustSize(parent);
}

QColor MWWidget::getHighlightColor(QColor color) const
{
    int newR = color.red() * 2;
    newR = newR > 255 ? 255 : newR;
    int newG = color.green() * 2;
    newG = newG > 255 ? 255 : newG;
    int newB = color.blue() * 2;
    newB = newB > 255 ? 255 : newB;
    int newA = color.alpha();

    return QColor(newR, newG, newB, newA);
}

void MWWidget::adjustPos(MWWidget * parent)
{
    if (parent)
    {
        setPos(parent->pos());
    }
}

void MWWidget::adjustSize(MWWidget * parent)
{
    if (parent)
    {
        int l, r, t, b;
        parent->getContentsMargins(l, r, t, b);

        if (parent->size().height() - (t + b) < m_size.height())
        {
            m_size.setHeight(parent->size().height() - (t + b));
        }

        if (parent->size().width() - (l + r) < m_size.width())
        {
            m_size.setWidth(parent->size().width() - (l + r));
        }
    }
}

QRectF MWWidget::boundingRect () const
{
    return QRectF(-m_size.width() / 2,
                  -m_size.height() / 2,
                   m_size.width(),
                   m_size.height());
}

void MWWidget::paintBackground(QPainter * painter)
{
    if (m_clickable && m_pressing)
    {
        painter->fillRect(boundingRect(),
                          getHighlightColor(m_bgColor));
    }
    else
    {
        painter->fillRect(boundingRect(), m_bgColor);
    }
}

void MWWidget::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                     QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    paintBackground(painter);
}

void MWWidget::setContentsMargins(int left, int right, int top, int bottom)
{
    m_cm[0] = left;
    m_cm[1] = right;
    m_cm[2] = top;
    m_cm[3] = bottom;
}

void MWWidget::getContentsMargins(int & left, int & right, int & top, int & bottom) const
{
    left   = m_cm[0];
    right  = m_cm[1];
    top    = m_cm[2];
    bottom = m_cm[3];
}

QSizeF MWWidget::size() const
{
    return m_size;
}

void MWWidget::setSize(QSizeF size)
{
    m_size = size;
    adjustSize(m_parent);
    update();
}

void MWWidget::setBgColor(QColor color)
{
    m_bgColor  = color;
    update();
}

QColor MWWidget::bgColor() const
{
    return m_bgColor;
}

void MWWidget::setFgColor(QColor color)
{
    m_fgColor = color;
    update();
}

QColor MWWidget::fgColor() const
{
    return m_fgColor;
}

void MWWidget::setClickable(bool enable)
{
    m_clickable = enable;
}

bool MWWidget::clickable() const
{
    return m_clickable;
}

void MWWidget::setPressing(bool pressing)
{
    m_pressing = pressing;
}

bool MWWidget::pressing() const
{
    return m_pressing;
}

void MWWidget::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    Q_UNUSED(event)

    if (m_clickable)
    {
        m_pressing = true;
        update();
    }
}

void MWWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    Q_UNUSED(event)

    if (m_clickable && m_pressing)
    {
        m_pressing = false;
        emit clicked();
        update();
    }
}
