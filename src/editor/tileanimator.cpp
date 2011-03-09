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

#include "tileanimator.h"
#include "tracktile.h"

#include <QTimeLine>
#include <QGraphicsItemAnimation>

TileAnimator::TileAnimator(TrackTile * tile, QObject *parent) :
    QObject(parent),
    m_tile(tile),
    m_timer(new QTimeLine(500, this)),
    m_animation(new QGraphicsItemAnimation(this))
{
    m_timer->setFrameRange(0, 30);

    m_animation->setItem(m_tile);
    m_animation->setTimeLine(m_timer);
}

void TileAnimator::rotate90CW()
{
    if (m_timer->state() == QTimeLine::NotRunning)
    {
        m_animation->setRotationAt(0.0f, m_tile->rotation());
        m_animation->setRotationAt(1.0f, m_tile->rotation() + 90);

        m_timer->start();
    }
}

void TileAnimator::rotate90CCW()
{
    if (m_timer->state() == QTimeLine::NotRunning)
    {
        m_animation->setRotationAt(0.0f, m_tile->rotation());
        m_animation->setRotationAt(1.0f, m_tile->rotation() - 90);

        m_timer->start();
    }
}

