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

#include "startlightsoverlay.hpp"
#include "startlights.hpp"

#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTextureManager"

StartlightsOverlay::StartlightsOverlay(Startlights & model)
  : m_startLightOn(MCTextureManager::instance().surface("startLightOn"))
  , m_startLightOff(MCTextureManager::instance().surface("startLightOff"))
  , m_startLightGlow(MCTextureManager::instance().surface("startLightGlow"))
  , m_model(model)
{
}

void StartlightsOverlay::renderLights(MCUint rows, MCUint litRows) const
{
    const MCUint cols = 8;

    const MCFloat x = m_model.pos().i() - (cols - 1) * m_startLightOn.width()  / 2;
    const MCFloat y = m_model.pos().j() - (rows - 1) * m_startLightOn.height() / 2;
    const MCFloat h = rows * m_startLightOn.height();

    // Body
    for (MCUint row = 0; row < rows; row++)
    {
        for (MCUint col = 0; col < cols; col++)
        {
            if (row < litRows)
            {
                m_startLightOn.render(
                    nullptr,
                    x + col * m_startLightOn.width(),
                    y + h - row * m_startLightOn.height(),
                    0,
                    0);
            }
            else
            {
                m_startLightOff.render(
                    nullptr,
                    x + col * m_startLightOff.width(),
                    y + h - row * m_startLightOff.height(),
                    0,
                    0);
            }
        }
    }

    // Glow
    for (MCUint row = 0; row < rows; row++)
    {
        for (MCUint col = 0; col < cols; col++)
        {
            if (row < litRows)
            {
                m_startLightGlow.render(
                    nullptr,
                    x + col * m_startLightOn.width(),
                    y + h - row * m_startLightOn.height(),
                    0,
                    0);
            }
        }
    }
}

void StartlightsOverlay::setDimensions(MCUint width, MCUint height)
{
    OverlayBase::setDimensions(width, height);
    m_model.setDimensions(width, height);
}

void StartlightsOverlay::render()
{
    switch (m_model.state())
    {
    case Startlights::LightsFirstRow:
        renderLights(3, 1);
        break;
    case Startlights::LightsSecondRow:
        renderLights(3, 2);
        break;
    case Startlights::LightsThirdRow:
        renderLights(3, 3);
        break;
    case Startlights::LightsGo:
    case Startlights::LightsAppear:
    case Startlights::LightsDisappear:
        renderLights(3, 0);
        break;
    case Startlights::LightsEnd:
        break;
    default:
        break;
    }
}
