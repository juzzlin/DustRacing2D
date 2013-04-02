// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "slidefrictiongenerator.hpp"

#include <MCObject>
#include <MCMathUtil>
#include <MCTrigonom>

#include <algorithm>

namespace
{
    const MCFloat FRICTION_MIN_SPEED = 0.001f;
    const MCFloat SLIDE_FACTOR       = 0.1f;
    const MCFloat FRICTION_LIMIT     = 4.0f;
}

SlideFrictionGenerator::SlideFrictionGenerator(
    MCFloat coeff, MCFloat gravity)
: MCFrictionGenerator(coeff, coeff, gravity)
, m_tireWearOutFactor(1.0)
{}

void SlideFrictionGenerator::setTireWearOutFactor(MCFloat value)
{
    m_tireWearOutFactor = value;
    m_tireWearOutFactor = std::max(0.33f, m_tireWearOutFactor);
    m_tireWearOutFactor = std::min(1.00f, m_tireWearOutFactor);
}

void SlideFrictionGenerator::updateForce(MCObject & object)
{
    const MCFloat bodyNormalAngle = object.angle() + 90;
    const MCVector2d<MCFloat> n(
        MCTrigonom::cos(bodyNormalAngle), MCTrigonom::sin(bodyNormalAngle));
    const MCVector2d<MCFloat> & v = object.velocity();
    const MCVector2d<MCFloat>   s = MCMathUtil::projection(v, n);

    if (s.lengthFast() > FRICTION_MIN_SPEED)
    {
        MCVector2d<MCFloat> impulse = -s * coeffLin();
        impulse.clampFast(FRICTION_LIMIT * m_tireWearOutFactor);
        object.addLinearImpulse(impulse * coeffLin() * SLIDE_FACTOR);
    }
}
