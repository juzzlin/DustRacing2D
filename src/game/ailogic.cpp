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
  , m_lastDiff(0)
  , m_integrate(0)
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

void AiLogic::steer(TrackTile & targetTile, TrackTile & currentTile)
{
    // Initial target coordinates
    MCVector3dF target(targetTile.location().x(), targetTile.location().y());

    // Take line hints into account
    if (currentTile.computerHint() == TrackTile::CH_LEFT_LINE)
    {
        target -= MCVector3dF(TrackTile::TILE_W / 2, 0);
    }
    else if (currentTile.computerHint() == TrackTile::CH_RIGHT_LINE)
    {
        target += MCVector3dF(TrackTile::TILE_W / 2, 0);
    }

    // A simple PID-controller
    target -= MCVector3dF(m_car.location());
    const MCFloat crossProd = (target.normalizedFast() * m_car.velocity().normalizedFast()).k();
    MCFloat adjust = 1.0f * crossProd * 1e5 + 1.0f * (crossProd - m_lastDiff) * 1e6;

    // Clamp the value
    const MCFloat maxAdjust = 1e6;
    if (adjust > maxAdjust) adjust  = maxAdjust;
    if (adjust < -maxAdjust) adjust = -maxAdjust;

    // Set turning moment and turn
    m_car.setTurningMoment(std::fabs(adjust));
    if (crossProd < -0.1f)
    {
        m_car.turnLeft();
    }
    else if (crossProd > 0.1f)
    {
        m_car.turnRight();
    }

    // Store the last difference
    m_lastDiff = crossProd;

    // Braking / acceleration logic

    bool accelerate = true;
    bool brake      = false;

    if (currentTile.computerHint() == TrackTile::CH_FIRST_BEFORE_CORNER)
    {
        if (m_car.speedInKmh() > 50)
        {
            brake = true;
        }
    }

    if (currentTile.tileType() == "corner90")
    {
        if (m_car.speedInKmh() > 50)
        {
            accelerate = false;
        }
    }

    if (currentTile.tileType() == "corner45Left" || currentTile.tileType() == "corner45Right")
    {
        if (m_car.speedInKmh() > 75)
        {
            accelerate = false;
        }
    }

    if (m_car.speedInKmh() < 25)
    {
        accelerate = true;
        brake = false;
    }

    if (std::abs(currentTile.matrixLocation().x() - targetTile.matrixLocation().x()) > 2 &&
        currentTile.matrixLocation().y() == targetTile.matrixLocation().y())
    {
        brake = false;
        accelerate = true;
    }

    if (std::abs(currentTile.matrixLocation().y() - targetTile.matrixLocation().y()) > 2 &&
        currentTile.matrixLocation().x() == targetTile.matrixLocation().x())
    {
        brake = false;
        accelerate = true;
    }

    if (brake)
    {
        m_car.brake();
    }
    else if (accelerate)
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
