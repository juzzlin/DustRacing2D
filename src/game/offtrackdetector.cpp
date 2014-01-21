// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "offtrackdetector.hpp"
#include "car.hpp"
#include "track.hpp"
#include "tracktile.hpp"

#include <MCTrigonom>
#include <cassert>

OffTrackDetector::OffTrackDetector(Car & car)
: m_car(car)
, m_tileWLimit(TrackTile::TILE_W / 2 - TrackTile::TILE_W / 10)
, m_tileHLimit(TrackTile::TILE_H / 2 - TrackTile::TILE_H / 10)
{
}

void OffTrackDetector::setTrack(Track & track)
{
    m_pTrack = &track;
}

void OffTrackDetector::update()
{
    assert(m_pTrack);

    {
        const MCVector3dF leftFrontTirePos(m_car.leftFrontTireLocation());
        TrackTile & tile = *m_pTrack->trackTileAtLocation(
            leftFrontTirePos.i(), leftFrontTirePos.j());

        m_car.setLeftSideOffTrack(false);

        if (isOffTrack(leftFrontTirePos, tile))
        {
            m_car.setLeftSideOffTrack(true);
        }
    }

    {
        const MCVector3dF rightFrontTirePos(m_car.rightFrontTireLocation());
        TrackTile & tile = *m_pTrack->trackTileAtLocation(
            rightFrontTirePos.i(), rightFrontTirePos.j());

        m_car.setRightSideOffTrack(false);

        if (isOffTrack(rightFrontTirePos, tile))
        {
            m_car.setRightSideOffTrack(true);
        }
    }
}

bool OffTrackDetector::isOffTrack(MCVector2dF tire, const TrackTile & tile) const
{
    if (!tile.hasAsphalt())
    {
        return true;
    }
    else if (
        tile.tileTypeEnum() == TrackTile::TT_STRAIGHT ||
        tile.tileTypeEnum() == TrackTile::TT_FINISH)
    {
        if ((tile.rotation() + 90) % 180 == 0)
        {
            const MCFloat y = tire.j();
            if (y > tile.location().y() + m_tileHLimit ||
                y < tile.location().y() - m_tileHLimit)
            {
                return true;
            }
        }
        else if (tile.rotation() % 180 == 0)
        {
            const MCFloat x = tire.i();
            if (x > tile.location().x() + m_tileWLimit ||
                x < tile.location().x() - m_tileWLimit)
            {
                return true;
            }
        }
    }
    else if (tile.tileTypeEnum() == TrackTile::TT_STRAIGHT_45_MALE)
    {
        const MCVector2dF diff = tire - MCVector2dF(tile.location().x(), tile.location().y());
        MCVector2dF rotatedDiff;
        MCTrigonom::rotatedVector(diff, rotatedDiff, tile.rotation() - 45);

        if (rotatedDiff.j() > m_tileHLimit || rotatedDiff.j() < -m_tileHLimit)
        {
            return true;
        }
    }
    else if (
        tile.tileTypeEnum() == TrackTile::TT_STRAIGHT_45_FEMALE)
    {
        const MCVector2dF diff = tire - MCVector2dF(tile.location().x(), tile.location().y());
        MCVector2dF rotatedDiff;
        MCTrigonom::rotatedVector(diff, rotatedDiff, 360 - tile.rotation() - 45);

        if (rotatedDiff.j() < m_tileHLimit)
        {
            return true;
        }
    }

    return false;
}
