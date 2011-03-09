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

TileAnimator::TileAnimator(TrackTile * tile, QObject *parent) :
    QObject(parent),
    m_tile(tile)
{}

#include <QDebug>
void TileAnimator::rotate90CW()
{
    qDebug("rotateCW");
}

void TileAnimator::rotate90CCW()
{
    qDebug("rotateCCW");
}

