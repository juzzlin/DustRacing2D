// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "ailogic.hpp"
#include "car.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
#include "../common/route.hpp"
#include "../common/tracktilebase.hpp"

#include "MiniCore/Core/MCTrigonom"

AiLogic::AiLogic(Car & car)
  : m_car(car)
  , m_track(nullptr)
  , m_route(nullptr)
  , m_targetIndex(0)
{
}

void AiLogic::update()
{
    if (m_track)
    {
        m_car.clearStatuses();

        TrackTile * currentTile = m_track->trackTileAtLocation(
            m_car.location().i(), m_car.location().j());
        TrackTile * targetTile  = dynamic_cast<TrackTile *>(m_route->get(m_targetIndex));

        if (currentTile->routeIndex() == targetTile->routeIndex())
        {
            if (++m_targetIndex >= m_route->length())
            {
                m_targetIndex = 0;
            }
        }

        steer(*targetTile, *currentTile);
    }
}

void AiLogic::steer(TrackTile & targetTile, TrackTile & currentTile) const
{
    MCVector2dF target(
        targetTile.location().x(),
        targetTile.location().y());

    if (targetTile.matrixLocation().x() == currentTile.matrixLocation().x())
    {
        target.setI(m_car.location().i());
    }

    if (targetTile.matrixLocation().y() == currentTile.matrixLocation().y())
    {
        target.setJ(m_car.location().j());
    }

    target -= MCVector2dF(m_car.location());

    const MCFloat crossProd =
        target.normalizedFast() *
        MCVector2dF(
            MCTrigonom::cos(m_car.angle()),
            MCTrigonom::sin(m_car.angle()));

    if (crossProd < 0.05f)
    {
        m_car.turnLeft();
    }
    else if (crossProd > -0.05f)
    {
        m_car.turnRight();
    }

    bool accelerate = true;
    bool brake      = false;
    if (currentTile.computerHint() == TrackTile::CH_SECOND_BEFORE_CORNER)
    {
        if (m_car.speedInKmh() > 100)
        {
            brake = true;
            accelerate = false;
        }
    }
    else if (currentTile.computerHint() == TrackTile::CH_FIRST_BEFORE_CORNER)
    {
        accelerate = false;
    }

    if (m_car.speedInKmh() < 50)
    {
        accelerate = true;
        brake = false;
    }

    if (brake)
    {
        m_car.brake();
    }
    else if (accelerate && crossProd < 0.5f && crossProd > -0.5f)
    {
        m_car.accelerate();
    }
}

void AiLogic::setTrack(Track & track)
{
    m_track = &track;
    m_route = &track.trackData().route();
}

AiLogic::~AiLogic()
{
}
