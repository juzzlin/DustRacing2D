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

#include "mccamera.hh"

MCCamera::MCCamera(MCFloat w, MCFloat h, MCFloat x, MCFloat y, MCFloat maxX, MCFloat maxY) :
  m_w(w),
  m_h(h),
  m_halfW(w / 2),
  m_halfH(h / 2),
  m_x(x),
  m_y(y),
  m_maxX(maxX),
  m_maxY(maxY)
{}

void MCCamera::setPos(MCFloat x, MCFloat y)
{
  if (x < m_halfW)
  {
    x = m_halfW;
  }
  else if (x > m_maxX - m_halfW)
  {
    x = m_maxX - m_halfW;
  }

  m_x = x;

  if (y < m_halfH)
  {
    y = m_halfH;
  }
  else if (y > m_maxY - m_halfH)
  {
    y = m_maxY - m_halfH;
  }

  m_y = y;
}

MCFloat MCCamera::width() const
{
  return m_w;
}

MCFloat MCCamera::height() const
{
  return m_h;
}

MCBBox<MCFloat> MCCamera::bbox() const
{
  return MCBBox<MCFloat>(m_x - m_halfW, m_y - m_halfH, m_x + m_halfW, m_y + m_halfH);
}
