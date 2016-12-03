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

#ifndef MCSURFACEMETADATA_HH
#define MCSURFACEMETADATA_HH

#include "mctypes.hh"
#include "mcvector2d.hh"

#include <MCGLEW> // For GLenum
#include <map>
#include <string>

//! Surface metadata structure used by MCSurfaceConfigLoader.
struct MCSurfaceMetaData
{
    //! Handle of the surface
    std::string handle;

    //! Image path
    std::string imagePath;

    //! Handle for the second texture if multitexturing is wanted.
    std::string handle2;

    //! Handle for the third texture if multitexturing is wanted.
    std::string handle3;

    //! Height
    std::pair<int, bool> height;

    //! Width
    std::pair<int, bool> width;

    //! Possible colorkey setting
    struct ColorKey
    {
        //! red, green, blue
        MCUint m_r, m_g, m_b;
    };

    //! Colorkey, if set
    ColorKey colorKey;

    //! True, if colorkey was set
    bool colorKeySet = false;

    //! Possible alphaBlend setting
    struct AlphaBlend
    {
        //! Src blend function
        GLenum m_src;

        //! Dest blend function
        GLenum m_dst;
    };

    //! Alpha blend settings, if set
    std::pair<AlphaBlend, bool> alphaBlend;

    /*! Pixels below this value are discarded, if set.
     *  This is currently a pre-process step so texture filters
     *  might affect this. */
    std::pair<float, bool> alphaClamp;

    //! True if X-Axis mirroring is wanted
    bool xAxisMirror = false;

    //! True if Y-Axis mirroring is wanted
    bool yAxisMirror = false;

    //! Min filter value
    std::pair<GLint, bool> minFilter;

    //! Mag filter value
    std::pair<GLint, bool> magFilter;

    //! Texture wrap s value
    std::pair<GLint, bool> wrapS;

    //! Texture wrap t value
    std::pair<GLint, bool> wrapT;

    //! Specular coeff
    std::pair<GLfloat, bool> specularCoeff;

    //! Z-coordinate of vertex[0].
    float z0 = 0.0f;

    //! Z-coordinate of vertex[1].
    float z1 = 0.0f;

    //! Z-coordinate of vertex[2].
    float z2 = 0.0f;

    //! Z-coordinate of vertex[3].
    float z3 = 0.0f;

    int sizeDivider = 1;
};

#endif // MCSURFACEMETADATA_HH
