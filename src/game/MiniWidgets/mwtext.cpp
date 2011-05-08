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
#include "mwtext.h"

MWText::MWText(const QString & text, QSizeF size, MWWidget * parent) :
    MWWidget(size, parent),
    m_text(text),
    m_textColor(QColor(0, 0, 0, 255))
{}

QString MWText::text() const
{
    return m_text;
}

void MWText::setText(const QString & text)
{
    m_text = text;
    update();
}

void MWText::setTextColor(QColor color)
{
    m_textColor = color;
    update();
}

QColor MWText::textColor() const
{
    return m_textColor;
}

void MWText::paint(QPainter * painter,
                   const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    MWWidget::paintBackground(painter, option, widget);

    QPen pen;
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(m_textColor);
    painter->setPen(pen);

    QFont font;
    font.setPixelSize(size().height() / 2);
    painter->setFont(font);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_text);

    painter->restore();
}
