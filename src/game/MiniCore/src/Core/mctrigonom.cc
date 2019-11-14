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
} // namespace

//! Implementation class for MCTrigonom
class MCTrigonomImpl
{
public:
    MCTrigonomImpl();

private:
    std::array<float, LUT_SIZE> m_sin;
    std::array<float, LUT_SIZE> m_cos;
    friend class MCTrigonom;
};

std::unique_ptr<MCTrigonomImpl> const MCTrigonom::m_pImpl(new MCTrigonomImpl);

MCTrigonomImpl::MCTrigonomImpl()
{
    for (size_t i = 0; i < LUT_SIZE; i++)
    {
        MCTrigonomImpl::m_sin.at(i) =
          std::sin(MCTrigonom::degToRad(static_cast<float>(i / SCALE - LUT_SIZE / 2)));
        MCTrigonomImpl::m_cos.at(i) =
          std::cos(MCTrigonom::degToRad(static_cast<float>(i / SCALE - LUT_SIZE / 2)));
    }
}

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
    const size_t index = static_cast<size_t>(angle * SCALE) + LUT_SIZE / 2;
    if (index < LUT_SIZE)
    {
        return m_pImpl->m_sin[index];
    }
    return std::sin(degToRad(angle));
}

float MCTrigonom::cos(float angle)
{
    const size_t index = static_cast<size_t>(angle * SCALE) + LUT_SIZE / 2;
    if (index < LUT_SIZE)
    {
        return m_pImpl->m_cos[index];
    }
    return std::cos(degToRad(angle));
}
