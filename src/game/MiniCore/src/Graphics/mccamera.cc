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

#include "mccamera.hh"
#include "mcvector2d.hh"

MCCamera::MCCamera()
{
    init(0, 0, 0, 0, 0, 0);
}

MCCamera::MCCamera(float w, float h, float x, float y, float maxX, float maxY)
{
    init(w, h, x, y, maxX, maxY);
}

void MCCamera::init(float w, float h, float x, float y, float maxX, float maxY)
{
    m_w = w;
    m_h = h;
    m_halfW = w / 2;
    m_halfH = h / 2;
    m_maxX = maxX;
    m_maxY = maxY;

    setPos(x, y);
}

void MCCamera::setPos(float x, float y)
{
    setPos(MCVector2dF(x, y));
}

void MCCamera::setPos(const MCVector2dF & pos)
{
    float x = pos.i();
    float y = pos.j();

    if (x < m_halfW)
    {
        x = m_halfW;
    }
    else if (x > m_maxX - m_halfW)
    {
        x = m_maxX - m_halfW;
    }

    m_pos.setI(x);

    if (y < m_halfH)
    {
        y = m_halfH;
    }
    else if (y > m_maxY - m_halfH)
    {
        y = m_maxY - m_halfH;
    }

    m_pos.setJ(y);
}

const MCVector2dF & MCCamera::pos() const
{
    return m_pos;
}

float MCCamera::x() const
{
    return m_pos.i();
}

float MCCamera::y() const
{
    return m_pos.j();
}

float MCCamera::width() const
{
    return m_w;
}

float MCCamera::height() const
{
    return m_h;
}

MCBBox<float> MCCamera::bbox() const
{
    return MCBBox<float>(
      m_pos.i() - m_halfW, m_pos.j() - m_halfH, m_pos.i() + m_halfW, m_pos.j() + m_halfH);
}
