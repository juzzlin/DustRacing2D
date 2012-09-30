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

#ifndef MCSURFACEDATA_HH
#define MCSURFACEDATA_HH

#include "mctypes.hh"
#include "mcvector2d.hh"

#include <GL/gl.h> // For GLenum
#include <string>

//! Surface data structure used by MCTextureParser.
struct MCSurfaceData
{
    MCSurfaceData()
    : centerSet(false)
    , heightSet(false)
    , widthSet(false)
    , colorKeySet(false)
    , alphaTestSet(false)
    , alphaBlendSet(false)
    , xAxisMirror(false)
    , minFilterSet(false)
    , magFilterSet(false)
    , z0(0)
    , z1(0)
    , z2(0)
    , z3(0)
    {}

    //! Handle of the texture
    std::string handle;

    //! Image path
    std::string imagePath;

    //! True if center was set
    bool centerSet;

    //! Center coordinate, if set
    MCVector2d<int> center;

    //! True if height was set
    bool heightSet;

    //! Height
    int height;

    //! True if width was set
    bool widthSet;

    //! Width
    int width;

    //! Possible colorkey setting
    struct ColorKey
    {
        //! red, green, blue
        MCUint m_r, m_g, m_b;
    };

    //! Colorkey, if set
    ColorKey colorKey;

    //! True, if colorkey was set
    bool colorKeySet;

    //! Possible alphaTest setting
    struct AlphaTest
    {
        //! Alpha test function
        GLenum m_function;

        //! Alpha test threshold
        GLclampf m_threshold;
    };

    //! Alpha test settings, if set
    AlphaTest alphaTest;

    //! True, if alpha test was set
    bool alphaTestSet;

    //! Possible alphaBlend setting
    struct AlphaBlend
    {
        //! Src blend function
        GLenum m_src;

        //! Dest blend function
        GLenum m_dst;
    };

    //! Alpha blend settings, if set
    AlphaBlend alphaBlend;

    //! True, if alpha blend was set
    bool alphaBlendSet;

    //! True if X-Axis mirroring is wanted
    bool xAxisMirror;

    //! Min filter value
    GLint minFilter;

    //! True, if min filter was set
    bool minFilterSet;

    //! Mag filter value
    GLint magFilter;

    //! True, if mag filter was set
    bool magFilterSet;

    //! Z-coordinate of vertex[0].
    int z0;

    //! Z-coordinate of vertex[1].
    int z1;

    //! Z-coordinate of vertex[2].
    int z2;

    //! Z-coordinate of vertex[3].
    int z3;
};

#endif // MCSURFACEDATA_HH
