// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include "ai.hpp"
#include "../common/route.hpp"
#include "../common/tracktilebase.hpp"
#include "car.hpp"
#include "race.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <MCRandom>
#include <MCTrigonom>

AI::AI(Car & car, std::shared_ptr<Race> race)
  : m_car(car)
  , m_race(race)
  , m_lastDiff(0)
  , m_lastTargetNodeIndex(0)
{
}

Car & AI::car() const
{
    return m_car;
}

void AI::update(bool isRaceCompleted)
{
    if (m_track)
    {
        if (m_lastTargetNodeIndex != m_race->getCurrentTargetNodeIndex(m_car))
        {
            setRandomTolerance();
        }

        const Route & route = m_track->trackData().route();
        steerControl(route.get(m_race->getCurrentTargetNodeIndex(m_car)));

        speedControl(*m_track->trackTileAtLocation(m_car.location().i(), m_car.location().j()), isRaceCompleted);

        m_lastTargetNodeIndex = m_race->getCurrentTargetNodeIndex(m_car);
    }
}

void AI::setRandomTolerance()
{
    m_randomTolerance = MCRandom::randomVector2d() * TrackTileBase::width() / 8;
}

void AI::steerControl(TargetNodeBasePtr targetNode)
{
    // Initial target coordinates
    MCVector3dF target(static_cast<float>(targetNode->location().x()), static_cast<float>(targetNode->location().y()));
    target -= MCVector3dF(m_car.location() + MCVector3dF(m_randomTolerance));

    const float angle = MCTrigonom::radToDeg(std::atan2(target.j(), target.i()));
    const float cur = static_cast<int>(m_car.angle()) % 360;
    float diff = angle - cur;

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
    float control = diff * 0.025f + (diff - m_lastDiff) * 0.025f;
    const float maxControl = 1.5;
    control = control < 0 ? -control : control;
    control = control > maxControl ? maxControl : control;

    const float maxDelta = 3.0;
    if (diff < -maxDelta)
    {
        m_car.steer(Car::Steer::Right, control);
    }
    else if (diff > maxDelta)
    {
        m_car.steer(Car::Steer::Left, control);
    }

    // Store the last difference
    m_lastDiff = diff;
}

void AI::speedControl(TrackTile & currentTile, bool isRaceCompleted)
{
    // TODO: Maybe it'd be possible to adjust speed according to
    // the difference between current and target angles so that
    // computer hints wouldn't be needed anymore..?

    // Braking / acceleration logic
    bool accelerate = true;
    bool brake = false;

    const float absSpeed = m_car.absSpeed();

    // The following speed limits are experimentally defined.
    float scale = 0.9f;
    if (currentTile.computerHint() == TrackTile::ComputerHint::Brake)
    {
        if (absSpeed > 14.0f * scale)
        {
            brake = true;
        }
    }

    if (currentTile.computerHint() == TrackTile::ComputerHint::BrakeHard)
    {
        if (absSpeed > 9.5f * scale)
        {
            brake = true;
        }
    }

    if (currentTile.tileTypeEnum() == TrackTile::TileType::Corner90)
    {
        if (absSpeed > 7.0f * scale)
        {
            accelerate = false;
        }
    }

    if (currentTile.tileTypeEnum() == TrackTile::TileType::Corner45Left || currentTile.tileTypeEnum() == TrackTile::TileType::Corner45Right)
    {
        if (absSpeed > 8.3f * scale)
        {
            accelerate = false;
        }
    }

    if (isRaceCompleted)
    {
        // Cool down lap speed (should be greater than tire spin threshold)
        if (absSpeed > 5.0f)
        {
            accelerate = false;
        }
    }
    else
    {
        if (absSpeed < 3.6f * scale)
        {
            accelerate = true;
            brake = false;
        }
    }

    if (brake)
    {
        m_car.setAcceleratorEnabled(false);
        m_car.setBrakeEnabled(true);
    }
    else if (accelerate)
    {
        m_car.setAcceleratorEnabled(true);
        m_car.setBrakeEnabled(false);
    }
    else
    {
        m_car.setAcceleratorEnabled(false);
        m_car.setBrakeEnabled(false);
    }
}

void AI::setTrack(std::shared_ptr<Track> track)
{
    m_track = track;
}
