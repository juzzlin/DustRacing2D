// This file is part of Dust Racing (DustRAC).
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

#include "../common/map.h"
#include "../common/trackdata.h"
#include "track.h"
#include "tracktile.h"
#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCSurface"
#include <QImage>
#include <QGLWidget>

Track::Track(TrackData * pTrackData)
: m_pTrackData(pTrackData)
, m_rows(m_pTrackData->map().rows())
, m_cols(m_pTrackData->map().cols())
, m_width(m_cols * TrackTile::TILE_W)
, m_height(m_rows * TrackTile::TILE_H)
{
}

MCUint Track::width() const
{
    return m_width;
}

MCUint Track::height() const
{
    return m_height;
}

const TrackData & Track::trackData() const
{
    return *m_pTrackData;
}

void Track::calculateVisibleIndices(const MCBBox<int> & r,
                                    MCUint & i0, MCUint & i2, MCUint & j0, MCUint & j2)

{
  // Calculate which tiles are visible in the Camera window:
  // columns from i0 to i2 and rows from j0 to j2.

  // X low index
  i0 = r.x1() * m_cols / m_width;
  i0 = i0 >= m_cols ? 0 : i0;

  // X high index
  i2 = r.x2() * m_cols / m_width;
  i2 = i2  >= m_cols ? m_cols - 1 : i2;

  // Y low index
  j0 = r.y1() * m_rows / m_height;
  j0 = j0 >= m_rows ? 0 : j0;

  // Y high index
  j2 = r.y2() * m_rows / m_height;
  j2 = j2  >= m_rows ? m_rows - 1 : j2;
}

void Track::render(MCCamera * pCamera)
{
    const Map & rMap = m_pTrackData->map();

    // Get the Camera window
    MCBBox<MCFloat> cameraBox(pCamera->bbox());

    // Calculate which tiles are visible
    MCUint i2, j2, i0, j0;
    calculateVisibleIndices(cameraBox, i0, i2, j0, j2);

    // Set the default color
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Tile coordinates
    int x, y;
    const int w = TrackTile::TILE_W;
    const int h = TrackTile::TILE_H;

    // Loop through the visible tile matrix and draw the tiles
    y = j0 * h;
    for (MCUint j = j0; j <= j2; j++)
    {
        x = i0 * w;
        for (MCUint i = i0; i <= i2; i++)
        {
            if (TrackTile * pTile = rMap.getTile(i, j))
            {
                if (MCSurface * pSurface = pTile->surface())
                {
                    // Calculate absolute coordinates to be
                    // used in the rendering
                    int X = x - cameraBox.x1();
                    int Y = y - cameraBox.y1();

                    glPushMatrix();

                    // Bind the texture according to the tile
                    glBindTexture(GL_TEXTURE_2D, pSurface->handle());
                    glNormal3f(0.0f, 0.0f, 1.0f);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

                    glTranslated(X + w / 2, Y + h / 2, 0);
                    glRotated(pTile->rotation(), 0, 0, 1);

                    // Render the tile as a quad
                    glBegin(GL_QUADS);

                    glTexCoord2i(0, 0);
                    glVertex2i  (-w / 2, -h / 2);
                    glTexCoord2i(0, 1);
                    glVertex2i  (-w / 2, h / 2);
                    glTexCoord2i(1, 1);
                    glVertex2i  (w / 2, h / 2);
                    glTexCoord2i(1, 0);
                    glVertex2i  (w / 2, -h / 2);

                    glEnd();
                    glPopMatrix();
                }
            }

            x += w;
        }

        y += h;
    }
}

Track::~Track()
{
    delete m_pTrackData;
}
