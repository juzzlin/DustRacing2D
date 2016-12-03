// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcglambientlight.hh"

MCGLAmbientLight::MCGLAmbientLight(GLfloat r, GLfloat g, GLfloat b, GLfloat i)
: m_r(r)
, m_g(g)
, m_b(b)
, m_i(i)
{}

MCGLAmbientLight::MCGLAmbientLight()
: m_r(1.0f)
, m_g(1.0f)
, m_b(1.0f)
, m_i(1.0f)
{}

GLfloat MCGLAmbientLight::r() const
{
    return m_r;
}

GLfloat MCGLAmbientLight::g() const
{
    return m_g;
}

GLfloat MCGLAmbientLight::b() const
{
    return m_b;
}

GLfloat MCGLAmbientLight::i() const
{
    return m_i;
}

void MCGLAmbientLight::setR(GLfloat r)
{
    m_r = r;
}

void MCGLAmbientLight::setG(GLfloat g)
{
    m_g = g;
}

void MCGLAmbientLight::setB(GLfloat b)
{
    m_b = b;
}

void MCGLAmbientLight::setI(GLfloat i)
{
    m_i = i;
}
