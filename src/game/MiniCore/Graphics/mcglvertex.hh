// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCGLVERTEX_HPP
#define MCGLVERTEX_HPP

#include <MCGLEW>

class MCGLVertex
{
public:

    MCGLVertex()
    : m_x(0), m_y(0), m_z(0)
    {
    }

    MCGLVertex(GLfloat x, GLfloat y, GLfloat z = 0)
    : m_x(x), m_y(y), m_z(z)
    {
    }

    inline GLfloat x() const
    {
        return m_x;
    }

    inline GLfloat y() const
    {
        return m_y;
    }

    inline GLfloat z() const
    {
        return m_z;
    }

    inline void setX(GLfloat x)
    {
        m_x = x;
    }

    inline void setY(GLfloat y)
    {
        m_y = y;
    }

    inline void setZ(GLfloat z)
    {
        m_z = z;
    }

private:

    GLfloat m_x, m_y, m_z;
};

#endif // MCGLVERTEX_HPP
