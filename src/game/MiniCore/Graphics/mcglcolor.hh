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

#ifndef MCGLCOLOR_HH
#define MCGLCOLOR_HH

#include <MCGLEW>

class MCGLColor
{
public:

    explicit MCGLColor(GLfloat r = 1.0f, GLfloat g = 1.0f, GLfloat b = 1.0f, GLfloat a = 1.0f)
    : m_r(r), m_g(g), m_b(b), m_a(a)
    {
    }

    inline GLfloat r() const
    {
        return m_r;
    }

    inline GLfloat g() const
    {
        return m_g;
    }

    inline GLfloat b() const
    {
        return m_b;
    }

    inline GLfloat a() const
    {
        return m_a;
    }

    inline void setR(GLfloat r)
    {
        m_r = r;
    }

    inline void setG(GLfloat g)
    {
        m_g = g;
    }

    inline void setB(GLfloat b)
    {
        m_b = b;
    }

    inline void setA(GLfloat a)
    {
        m_a = a;
    }

private:

    GLfloat m_r, m_g, m_b, m_a;
};

#endif // MCGLCOLOR_HH
