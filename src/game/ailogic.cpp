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
{
}

Car & AiLogic::car() const
{
    return m_car;
}

void AiLogic::update(bool isRaceCompleted)
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

        steer(*targetTile, *currentTile, isRaceCompleted);
    }
}

void AiLogic::steer(
    TrackTile & targetTile, TrackTile & currentTile, bool isRaceCompleted)
{
    // Initial target coordinates
    MCVector3dF target(targetTile.location().x(), targetTile.location().y());

    // Take line hints into account
    if (currentTile.drivingLineHint() == TrackTile::DLH_LEFT)
    {
        target -= MCVector3dF(TrackTile::TILE_W / 3, 0);
    }
    else if (currentTile.drivingLineHint() == TrackTile::DLH_RIGHT)
    {
        target += MCVector3dF(TrackTile::TILE_W / 3, 0);
    }
    else if (currentTile.drivingLineHint() == TrackTile::DLH_TOP)
    {
        target += MCVector3dF(0, TrackTile::TILE_H / 3);
    }
    else if (currentTile.drivingLineHint() == TrackTile::DLH_BOTTOM)
    {
        target -= MCVector3dF(0, TrackTile::TILE_H / 3);
    }

    target -= MCVector3dF(m_car.location());

    MCFloat angle = MCTrigonom::radToDeg(std::atan2(target.j(), target.i()));
    MCFloat cur   = static_cast<int>(m_car.angle()) % 360;
    MCFloat diff  = angle - cur;

    bool ok = false;
    while (!ok)
    {
        if (diff > 180)
        {
            diff = diff - 360;
            ok = false;
        }
        else if (diff < -180)
        {
            diff = diff + 360;
            ok = false;
        }
        else
        {
            ok = true;
        }
    }

    // PID-controller. This makes the computer players to turn and react faster
    // than the human player, but hey...they are stupid.
    MCFloat control = diff * 0.01f + (diff - m_lastDiff) * 0.01f;
    const MCFloat maxControl = 1.0f;
    control = control < 0 ? -control : control;
    control = control > maxControl ? maxControl : control;
    m_car.setTurningImpulse(control);

    if (diff < -3.0f)
    {
        m_car.turnRight();
    }
    else if (diff > 3.0f)
    {
        m_car.turnLeft();
    }

    // Store the last difference
    m_lastDiff = diff;

    // Braking / acceleration logic

    bool accelerate = true;
    bool brake      = false;

    if (isRaceCompleted)
    {
        accelerate = false;
    }
    else
    {
        if (currentTile.computerHint() == TrackTile::CH_SECOND_BEFORE_CORNER)
        {
            if (m_car.speedInKmh() > 120)
            {
                brake = true;
            }
        }

        if (currentTile.computerHint() == TrackTile::CH_FIRST_BEFORE_CORNER)
        {
            if (m_car.speedInKmh() > 90)
            {
                brake = true;
            }
        }

        if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_90)
        {
            if (m_car.speedInKmh() > 60)
            {
                accelerate = false;
            }
        }

        if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_LEFT ||
                currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_RIGHT)
        {
            if (m_car.speedInKmh() > 120)
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
