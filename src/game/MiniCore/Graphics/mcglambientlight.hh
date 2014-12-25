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

#ifndef MCGLAMBIENTLIGHT_HH
#define MCGLAMBIENTLIGHT_HH

#include <MCGLEW>

//! Ambient light structure.
class MCGLAmbientLight
{
public:

    /*! Constructor.
     *  \param r Red.
     *  \param g Green.
     *  \param b Blue.
     *  \param i Intensity (0.0 - 1.0). */
    MCGLAmbientLight(GLfloat r, GLfloat g, GLfloat b, GLfloat i);

    MCGLAmbientLight();

    GLfloat r() const;

    GLfloat g() const;

    GLfloat b() const;

    GLfloat i() const;

    void setR(GLfloat r);

    void setG(GLfloat g);

    void setB(GLfloat b);

    void setI(GLfloat i);

private:

    GLfloat m_r;
    GLfloat m_g;
    GLfloat m_b;
    GLfloat m_i;
};

#endif // MCGLAMBIENTLIGHT_HH
