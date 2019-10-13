// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#include "animationcurve.hpp"

#include <cassert>
#include <cmath>

namespace MTFH {

AnimationCurve::AnimationCurve(int steps, int exp)
  : m_values(steps, 1)
{
    assert(steps > 2);
    assert(exp >= 1);

    float index = 0;
    for (float & value : m_values)
    {
        value = std::pow(index / (steps - 1), exp);
        index++;
    }
}

void AnimationCurve::step()
{
    if (m_index + 1 < static_cast<int>(m_values.size()))
    {
        m_index++;
    }
}

void AnimationCurve::reset()
{
    m_index = 0;
}

float AnimationCurve::value() const
{
    return m_values.at(m_index);
}

} // namespace MTFH
