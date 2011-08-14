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

shared_ptr<MCTrigonomImpl> const MCTrigonom::m_pImpl(new MCTrigonomImpl);

namespace
{
  const UINT LUT_SIZE = 361;
  const MCFloat PI = 3.1415926535897932384626;
}

MCTrigonomImpl::MCTrigonomImpl() :
    m_sin(LUT_SIZE, 0),
    m_cos(LUT_SIZE, 0)
{
  for (UINT i = 0; i < LUT_SIZE; i++) {
    MCTrigonomImpl::m_sin.at(i) = std::sin(TO_FLOAT(MCTrigonom::degToRad(i)));
    MCTrigonomImpl::m_cos.at(i) = std::cos(TO_FLOAT(MCTrigonom::degToRad(i)));
  }
}

MCFloat MCTrigonom::degToRad(MCFloat angle)
{
  const MCFloat DEG_TO_RAD(PI / 180.0);
  return angle * DEG_TO_RAD;
}

MCFloat MCTrigonom::radToDeg(MCFloat angle)
{
  const MCFloat RAD_TO_DEG(180.0 / PI);
  return angle * RAD_TO_DEG;
}

MCFloat MCTrigonom::sin(int angle)
{
  return MCTrigonom::m_pImpl->m_sin[angle % LUT_SIZE];
}

MCFloat MCTrigonom::cos(int angle)
{
  return MCTrigonom::m_pImpl->m_cos[angle % LUT_SIZE];
}

MCFloat MCTrigonom::transformedX(MCFloat x0, MCFloat y0, int angle)
{
  angle %= LUT_SIZE;
  return MCTrigonom::m_pImpl->m_cos[angle] * x0 - MCTrigonom::m_pImpl->m_sin[angle] * y0;
}

MCFloat MCTrigonom::transformedY(MCFloat x0, MCFloat y0, int angle)
{
  angle %= LUT_SIZE;
  return MCTrigonom::m_pImpl->m_sin[angle] * x0 + MCTrigonom::m_pImpl->m_cos[angle] * y0;
}
