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

#include "checkeredflag.hpp"
#include "renderer.hpp"

#include <MCAssetManager>
#include <MCGLColor>
#include <mcgltexcoord.hh>
#include <MCSurface>
#include <MCVector3d>

#include <cmath>

namespace {

const int FLAG_W = 32;
const int FLAG_H = 24;
const int V_SPACING = 20;

const size_t STRIP_COUNT = 16;

//! Wave height in pixels at the free end of the flag. The end at the pole stays still.
const float WAVE_AMPLITUDE = 2.5f;

//! Number of wave crests across the flag.
const float WAVE_COUNT = 1.25f;

//! Wave speed in radians per second.
const float WAVE_SPEED = 7.0f;

//! How much the slopes of the folds darken or lighten the flag.
const float FOLD_SHADING = 0.25f;

constexpr float TWO_PI = 6.2831853f;

} // namespace

CheckeredFlag::CheckeredFlag()
  : m_surface { MCAssetManager::surfaceManager().surface("checkeredFlag") }
  , m_startTime { std::chrono::steady_clock::now() }
{
    const float stripWidth { static_cast<float>(FLAG_W) / STRIP_COUNT };
    for (size_t i = 0; i < STRIP_COUNT; i++)
    {
        const float u0 { static_cast<float>(i) / STRIP_COUNT };
        const float u1 { static_cast<float>(i + 1) / STRIP_COUNT };
        const MCGLTexCoord texCoords[4] { { u0, 0 }, { u0, 1 }, { u1, 1 }, { u1, 0 } };
        const auto strip { std::make_shared<MCSurface>("checkeredFlagStrip", m_surface->material(), stripWidth, FLAG_H, texCoords) };
        strip->setShaderProgram(m_surface->shaderProgram());
        m_strips.push_back(strip);
    }
}

void CheckeredFlag::render()
{
    const float time { std::chrono::duration<float> { std::chrono::steady_clock::now() - m_startTime }.count() };
    const float stripWidth { static_cast<float>(FLAG_W) / STRIP_COUNT };
    const float left { static_cast<float>(width()) / 2 - static_cast<float>(FLAG_W) / 2 };
    const float centerY { static_cast<float>(height() - FLAG_H / 2 - V_SPACING) };

    for (size_t i = 0; i < m_strips.size(); i++)
    {
        // Relative distance from the pole: 0.0 at the pole and 1.0 at the free end.
        const float distance { (static_cast<float>(i) + 0.5f) / STRIP_COUNT };
        const float phase { distance * WAVE_COUNT * TWO_PI - time * WAVE_SPEED };
        const float displacement { WAVE_AMPLITUDE * distance * std::sin(phase) };
        const float shade { 1.0f - FOLD_SHADING * distance * std::cos(phase) };

        auto && strip { m_strips.at(i) };
        strip->setColor(MCGLColor { shade, shade, shade });
        strip->render(nullptr, MCVector3dF { left + stripWidth * (static_cast<float>(i) + 0.5f), centerY + displacement, 0 }, 0);
    }
}
