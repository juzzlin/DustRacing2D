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

#include "startlightsoverlay.hpp"
#include "startlights.hpp"

#include <MCAssetManager>
#include <MCGLColor>
#include <MCSurface>

StartlightsOverlay::StartlightsOverlay(Startlights & model)
: m_startLightOn(MCAssetManager::surfaceManager().surface("startLightOn"))
, m_startLightOnCorner(MCAssetManager::surfaceManager().surface("startLightOnCorner"))
, m_startLightOff(MCAssetManager::surfaceManager().surface("startLightOff"))
, m_startLightOffCorner(MCAssetManager::surfaceManager().surface("startLightOffCorner"))
, m_startLightGlow(MCAssetManager::surfaceManager().surface("startLightGlow"))
, m_model(model)
, m_alpha(1.0)
{
    m_startLightOff.setAlphaBlend(true);
    m_startLightOffCorner.setAlphaBlend(true);
    m_startLightGlow.setColor(MCGLColor(1.5f, 0.25f, 0.25f, 0.4f));
}

void StartlightsOverlay::renderLights(int rows, int litRows, float glowScale, bool glowAlways) const
{
    const int cols = 8;

    const MCFloat x = m_model.pos().i() - (cols - 1) * m_startLightOn.width()  / 2;
    const MCFloat y = m_model.pos().j() - (rows - 1) * m_startLightOn.height() / 2;
    const MCFloat h = rows * m_startLightOn.height();

    // Body
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            const MCVector3dF pos(
                x + col * m_startLightOff.width(),
                y + h - row * m_startLightOff.height());

            if (row < litRows)
            {
                if (row == 0 && col == 0)
                {
                    m_startLightOnCorner.render(nullptr, pos, 0);
                }
                else if (row == rows - 1 && col == 0)
                {
                    m_startLightOnCorner.render(nullptr, pos, 90);
                }
                else if (row == rows - 1 && col == cols - 1)
                {
                    m_startLightOnCorner.render(nullptr, pos, 180);
                }
                else if (row == 0 && col == cols - 1)
                {
                    m_startLightOnCorner.render(nullptr, pos, 270);
                }
                else
                {
                    m_startLightOn.render(nullptr, pos, 0);
                }
            }
            else
            {
                if (row == 0 && col == 0)
                {
                    m_startLightOffCorner.render(nullptr, pos, 0);
                }
                else if (row == rows - 1 && col == 0)
                {
                    m_startLightOffCorner.render(nullptr, pos, 90);
                }
                else if (row == rows - 1 && col == cols - 1)
                {
                    m_startLightOffCorner.render(nullptr, pos, 180);
                }
                else if (row == 0 && col == cols - 1)
                {
                    m_startLightOffCorner.render(nullptr, pos, 270);
                }
                else
                {
                    m_startLightOff.render(nullptr, pos, 0);
                }
            }
        }
    }

    // Glow
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            if (row < litRows || glowAlways)
            {
                m_startLightGlow.setScale(glowScale, glowScale, 1.0);
                m_startLightGlow.render(
                    nullptr,
                    MCVector3dF(
                        x + col * m_startLightOn.width(),
                        y + h - row * m_startLightOn.height()),
                    0);
            }
        }
    }
}

void StartlightsOverlay::setDimensions(int width, int height)
{
    OverlayBase::setDimensions(width, height);
    m_model.setDimensions(width, height);
}

void StartlightsOverlay::render()
{
    glDisable(GL_DEPTH_TEST);

    switch (m_model.state())
    {
    case Startlights::FirstRow:
        m_alpha = 1.0;
        renderLights(3, 1, m_model.glowScale());
        break;

    case Startlights::SecondRow:
        renderLights(3, 2, m_model.glowScale());
        break;

    case Startlights::ThirdRow:
        renderLights(3, 3, m_model.glowScale());
        break;

    case Startlights::Go:
        m_alpha *= 0.98;
        m_startLightOff.setColor(MCGLColor(1.0, 1.0, 1.0, m_alpha));
        m_startLightOffCorner.setColor(MCGLColor(1.0, 1.0, 1.0, m_alpha));
        renderLights(3, 0, m_model.glowScale(), true);
        break;

    case Startlights::Disappear:
    case Startlights::Appear:
        m_startLightOff.setColor(MCGLColor(1.0, 1.0, 1.0, 1.0));
        m_startLightOffCorner.setColor(MCGLColor(1.0, 1.0, 1.0, 1.0));
        renderLights(3, 0, m_model.glowScale());
        break;

    case Startlights::End:
        break;

    default:
        break;
    }
}
