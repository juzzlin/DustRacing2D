// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcfrictiongenerator.hh"
#include "mcfrictiongeneratorimpl.hh"
#include "mcmathutil.hh"
#include "mcobject.hh"
#include "mcshape.hh"

namespace
{
    const MCFloat FRICTION_SPEED_TH = 0.001f;
}

MCFrictionGeneratorImpl::MCFrictionGeneratorImpl(
    MCFloat coeffLin, MCFloat coeffRot, MCFloat gravity)
: m_coeffLin(coeffLin)
, m_coeffRot(coeffRot)
, m_gravity(gravity)
{}

MCFrictionGeneratorImpl::~MCFrictionGeneratorImpl()
{}

MCFrictionGenerator::MCFrictionGenerator(
    MCFloat coeffLin, MCFloat coeffRot, MCFloat gravity)
: m_pImpl(new MCFrictionGeneratorImpl(coeffLin, coeffRot, gravity))
{}

void MCFrictionGenerator::updateForce(MCObject & object)
{
    // Linear motion.
    const MCVector2d<MCFloat> v(object.velocity());
    MCFloat x = v.lengthFast();
    if (x > FRICTION_SPEED_TH && m_pImpl->m_coeffLin > 0.0f) {
        object.addForce(
        (-v / x) * m_pImpl->m_coeffLin * m_pImpl->m_gravity * object.mass());
    }

    // Approximated moment caused by rotational friction.
    if (object.shape()) {
        x = object.angularVelocity() * object.shape()->radius();
        if (MCMathUtil::abs(x) > FRICTION_SPEED_TH && m_pImpl->m_coeffRot > 0.0f) {
            object.addMoment(
                -x * m_pImpl->m_coeffRot * m_pImpl->m_gravity * object.mass());
        }
    }
}

MCFloat MCFrictionGenerator::coeffLin() const
{
    return m_pImpl->m_coeffLin;
}

MCFloat MCFrictionGenerator::coeffRot() const
{
    return m_pImpl->m_coeffRot;
}

MCFloat MCFrictionGenerator::gravity() const
{
    return m_pImpl->m_gravity;
}

MCFrictionGenerator::~MCFrictionGenerator()
{
    delete m_pImpl;
}

