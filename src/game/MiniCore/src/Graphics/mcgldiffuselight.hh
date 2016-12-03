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

#ifndef MCGLDIFFUSELIGHT_HH
#define MCGLDIFFUSELIGHT_HH

#include "mcglambientlight.hh"
#include "mcvector3d.hh"

//! Diffuse light structure.
class MCGLDiffuseLight : public MCGLAmbientLight
{
public:

    //! Constructor.
    MCGLDiffuseLight(
        MCVector3dF direction,
        GLfloat r, GLfloat g, GLfloat b, GLfloat i);

    MCGLDiffuseLight();

    const MCVector3dF & direction() const;

    void setDirection(MCVector3dF direction);

private:

    MCVector3dF m_direction;
};

#endif // MCGLDIFFUSELIGHT_HH
