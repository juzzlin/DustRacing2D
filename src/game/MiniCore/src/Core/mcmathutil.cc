// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcmathutil.hh"
#include "mctrigonom.hh"

MCFloat MCMathUtil::rotatedX(MCFloat x0, MCFloat y0, MCFloat angle)
{
    return MCTrigonom::cos(angle) * x0 -
        MCTrigonom::sin(angle) * y0;
}

MCFloat MCMathUtil::rotatedY(MCFloat x0, MCFloat y0, MCFloat angle)
{
    return MCTrigonom::sin(angle) * x0 +
        MCTrigonom::cos(angle) * y0;
}

void MCMathUtil::rotateVector(
    const MCVector2dF & v0, MCVector2dF & v1, MCFloat angle)
{
    const MCFloat sin = MCTrigonom::sin(angle);
    const MCFloat cos = MCTrigonom::cos(angle);

    v1.setI(cos * v0.i() - sin * v0.j());
    v1.setJ(sin * v0.i() + cos * v0.j());
}

MCVector2dF MCMathUtil::rotatedVector(const MCVector2dF & v0, MCFloat angle)
{
    const MCFloat sin = MCTrigonom::sin(angle);
    const MCFloat cos = MCTrigonom::cos(angle);

    return MCVector2dF(cos * v0.i() - sin * v0.j(), sin * v0.i() + cos * v0.j());
}

MCFloat MCMathUtil::distanceFromVector(const MCVector2dF & p, const MCVector2dF & v)
{
    return abs(p.dot(MCVector2dF(-v.j(), v.i()).normalized()));
}

bool MCMathUtil::crosses(const MCSegmentF & a, const MCSegmentF & b)
{
    const MCVector2dF a0a1(a.vertex1 - a.vertex0);
    const MCVector2dF a0b0(b.vertex0 - a.vertex0);
    const MCVector2dF a0b1(b.vertex1 - a.vertex0);

    if (sign(a0a1 % a0b0) != sign(a0a1 % a0b1))
    {
        const MCVector2dF b0b1(b.vertex1 - b.vertex0);
        const MCVector2dF b0a0(a.vertex0 - b.vertex0);
        const MCVector2dF b0a1(a.vertex1 - b.vertex0);

        if (sign(b0b1 % b0a0) != sign(b0b1 % b0a1))
        {
            return true;
        }
    }

    return false;
}
