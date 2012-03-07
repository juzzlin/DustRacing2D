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

#include "centrifugalforcegenerator.hpp"
#include "radius.hpp"
#include "MiniCore/Core/MCObject"

void CentrifugalForceGenerator::updateForce(MCObject & object)
{
    m_queue.push_back(object.location());

    if (m_queue.size() > 2)
    {
        Radius::CirclePoints cp;
        cp.x1 = m_queue[0].i();
        cp.y1 = m_queue[0].j();
        cp.x2 = m_queue[1].i();
        cp.y2 = m_queue[1].j();
        cp.x3 = m_queue[2].i();
        cp.y3 = m_queue[2].j();

        const MCFloat radius = Radius::calculate(cp);

        if (radius > 1 && radius < 1000) // Make things a bit more stable
        {
            const MCFloat amp = 5;
            const MCFloat velocity2 = (object.velocity() * amp).lengthSquared();
            const MCFloat force     = object.mass() * velocity2 / radius;

            const MCVector2dF v1(cp.x2 - cp.x1, cp.y2 - cp.y1);
            const MCVector2dF v2(cp.x3 - cp.x1, cp.y3 - cp.y1);

            static const MCVector3dF up(0, 0, 1);
            const MCVector3dF forceVect = object.velocity().normalizedFast() % up;

            if (v1 % v2 < 0)
            {
                object.addForce(forceVect * -force);
            }
            else
            {
                object.addForce(forceVect * force);
            }

        }

        m_queue.pop_front();
    }
}
