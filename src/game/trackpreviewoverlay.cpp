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

#include "trackpreviewoverlay.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include "MiniCore/Core/MCSurface"

#include "../common/mapbase.hpp"

TrackPreviewOverlay::TrackPreviewOverlay()
: m_track(nullptr)
{
}

void TrackPreviewOverlay::setTrack(Track * track)
{
    m_track = track;
}

void TrackPreviewOverlay::render()
{
    if (m_track)
    {
        const MapBase & rMap = m_track->trackData().map();

        const int previewW = width() / 2;
        const int previewH = height() / 2;

        // Set tileW and tileH so that they are squares
        MCFloat tileW = previewW / rMap.cols();
        MCFloat tileH = previewH / rMap.rows();

        if (tileW > tileH)
        {
            tileW = tileH;
        }
        else
        {
            tileH = tileW;
        }

        // Center the preview
        const int initX = width()  / 2 - rMap.cols() * tileW / 2;
        const int initY = height() / 2 - rMap.rows() * tileH / 2;

        // Loop through the visible tile matrix and draw the tiles
        MCFloat x, y;
        y = initY;
        const int j2 = rMap.rows();
        const int i2 = rMap.cols();
        for (int j = 0; j <= j2; j++)
        {
            x = initX;
            for (int i = 0; i <= i2; i++)
            {
                if (TrackTile * pTile = static_cast<TrackTile *>(rMap.getTile(i, j)))
                {
                    if (MCSurface * pSurface = pTile->previewSurface())
                    {
                        pSurface->renderScaled(
                            nullptr,
                            MCVector3dF(x + tileW / 2, y + tileH / 2, 0),
                            tileW / 2, tileH / 2,
                            pTile->rotation());
                    }
                }

                x += tileW;
            }

            y += tileH;
        }
    }
}
