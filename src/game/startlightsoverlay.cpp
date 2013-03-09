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
#include "renderer.hpp"

#include <MCAssetManager>
#include <MCSurface>

StartlightsOverlay::StartlightsOverlay(Startlights & model)
: m_startLightOn(MCAssetManager::surfaceManager().surface("startLightOn"))
, m_startLightOff(MCAssetManager::surfaceManager().surface("startLightOff"))
, m_startLightGlow(MCAssetManager::surfaceManager().surface("startLightGlow"))
, m_model(model)
{
    m_startLightOn.setShaderProgram(&Renderer::instance().program("master"));
    m_startLightOff.setShaderProgram(&Renderer::instance().program("master"));
    m_startLightGlow.setShaderProgram(&Renderer::instance().program("master"));
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
            if (row < litRows)
            {
                m_startLightOn.render(
                    nullptr,
                    MCVector3dF(
                        x + col * m_startLightOn.width(),
                        y + h - row * m_startLightOn.height()),
                    0);
            }
            else
            {
                m_startLightOff.render(
                    nullptr,
                    MCVector3dF(
                        x + col * m_startLightOff.width(),
                        y + h - row * m_startLightOff.height()),
                    0);
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
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);

    switch (m_model.state())
    {
    case Startlights::FirstRow:
        renderLights(3, 1, m_model.glowScale());
        break;

    case Startlights::SecondRow:
        renderLights(3, 2, m_model.glowScale());
        break;

    case Startlights::ThirdRow:
        renderLights(3, 3, m_model.glowScale());
        break;

    case Startlights::Go:
        renderLights(3, 0, m_model.glowScale(), true);
        break;

    case Startlights::Appear:
    case Startlights::Disappear:
        renderLights(3, 0, m_model.glowScale());
        break;

    case Startlights::End:
        break;

    default:
        break;
    }

    glPopAttrib();
}
