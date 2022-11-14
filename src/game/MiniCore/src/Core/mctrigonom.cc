// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2009 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mctrigonom.hh"
#include "mccast.hh"

#include <array>
#include <cmath>

namespace {
const size_t LUT_SIZE = 7200;
const float SCALE = 10;
const float PI = 3.1415926536f;

float normalize(const float value, const float start, const float end)
{
    const float width = end - start;
    const float offsetValue = value - start;

    return (offsetValue - (std::floor(offsetValue / width) * width)) + start;
}
} // namespace

struct MCTrigonom::Impl
{
    Impl()
    {
        for (size_t i = 0; i < LUT_SIZE; i++)
        {
            Impl::m_sin.at(i) =
              std::sin(MCTrigonom::degToRad(static_cast<float>(i / SCALE - LUT_SIZE / 2)));
            Impl::m_cos.at(i) =
              std::cos(MCTrigonom::degToRad(static_cast<float>(i / SCALE - LUT_SIZE / 2)));
        }
    }

private:
    std::array<float, LUT_SIZE> m_sin;
    std::array<float, LUT_SIZE> m_cos;

    friend class MCTrigonom;
};

std::unique_ptr<MCTrigonom::Impl> const MCTrigonom::m_impl(std::make_unique<MCTrigonom::Impl>());

float MCTrigonom::degToRad(float angle)
{
    return angle * PI / 180.0f;
}

float MCTrigonom::radToDeg(float angle)
{
    return angle * 180.0f / PI;
}

float MCTrigonom::sin(float angle)
{
    if (angle > 360.0f || angle < -360.0f)
    {
        angle = normalize(angle, -360.0f, 360.0f);
    }
    if (const size_t index = static_cast<size_t>((angle * SCALE) + (LUT_SIZE / 2)); index < LUT_SIZE)
    {
        return m_impl->m_sin[index];
    }
    return std::sin(degToRad(angle));
}

float MCTrigonom::cos(float angle)
{
    if (angle > 360.0f || angle < -360.0f)
    {
        angle = normalize(angle, -360.0f, 360.0f);
    }
    if (const size_t index = static_cast<size_t>((angle * SCALE) + (LUT_SIZE / 2)); index < LUT_SIZE)
    {
        return m_impl->m_cos[index];
    }
    return std::cos(degToRad(angle));
}
