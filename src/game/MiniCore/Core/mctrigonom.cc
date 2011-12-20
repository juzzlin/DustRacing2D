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
#include "mctrigonomimpl.hh"
#include "mccast.hh"

#include <cmath>

std::shared_ptr<MCTrigonomImpl> const MCTrigonom::m_pImpl(new MCTrigonomImpl);

namespace
{
    const MCUint  LutSize = 361;
    const MCFloat PI      = 3.1415926535897932384626;
}

MCTrigonomImpl::MCTrigonomImpl()
: m_sin(LutSize, 0)
, m_cos(LutSize, 0)
{
    for (MCUint i = 0; i < LutSize; i++) {
        MCTrigonomImpl::m_sin.at(i) =
            std::sin(TO_FLOAT(MCTrigonom::degToRad(i)));
        MCTrigonomImpl::m_cos.at(i) =
            std::cos(TO_FLOAT(MCTrigonom::degToRad(i)));
    }
}

MCFloat MCTrigonom::degToRad(MCFloat angle)
{
    static const MCFloat DegToRad(PI / 180.0);
    return angle * DegToRad;
}

MCFloat MCTrigonom::radToDeg(MCFloat angle)
{
    static const MCFloat RadToDeg(180.0 / PI);
    return angle * RadToDeg;
}

MCFloat MCTrigonom::sin(MCUint angle)
{
    return MCTrigonom::m_pImpl->m_sin[angle % LutSize];
}

MCFloat MCTrigonom::cos(MCUint angle)
{
    return MCTrigonom::m_pImpl->m_cos[angle % LutSize];
}

MCFloat MCTrigonom::rotatedX(MCFloat x0, MCFloat y0, MCUint angle)
{
    angle %= LutSize;
    return MCTrigonom::m_pImpl->m_cos[angle] * x0 -
        MCTrigonom::m_pImpl->m_sin[angle] * y0;
}

MCFloat MCTrigonom::rotatedY(MCFloat x0, MCFloat y0, MCUint angle)
{
    angle %= LutSize;
    return MCTrigonom::m_pImpl->m_sin[angle] * x0 +
        MCTrigonom::m_pImpl->m_cos[angle] * y0;
}

void MCTrigonom::rotated(
    MCFloat x0, MCFloat y0, MCFloat & x1, MCFloat & y1, MCUint angle)
{
    angle %= LutSize;
    y1 = MCTrigonom::m_pImpl->m_sin[angle] * x0 +
         MCTrigonom::m_pImpl->m_cos[angle] * y0;
    x1 = MCTrigonom::m_pImpl->m_cos[angle] * x0 -
         MCTrigonom::m_pImpl->m_sin[angle] * y0;
}

void MCTrigonom::rotated(
    const MCVector2d<MCFloat> & v0, MCVector2d<MCFloat> & v1,
    MCUint angle)
{
    angle %= LutSize;
    v1.setJ(MCTrigonom::m_pImpl->m_sin[angle] * v0.i() +
            MCTrigonom::m_pImpl->m_cos[angle] * v0.j());
    v1.setI(MCTrigonom::m_pImpl->m_cos[angle] * v0.i() -
            MCTrigonom::m_pImpl->m_sin[angle] * v0.j());
}
