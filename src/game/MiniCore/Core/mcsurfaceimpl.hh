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

#ifndef MCSURFACEIMPL_HH
#define MCSURFACEIMPL_HH

//! Implementation class for MCSurface
class MCSurfaceImpl
{
    MCSurfaceImpl(GLuint handle, MCFloat width, MCFloat height);
    virtual ~MCSurfaceImpl() {};

    MCBBox<MCFloat> inline rotatedBBox(MCVector2dFR pos, MCFloat angle);
    MCBBox<MCFloat> inline rotatedScaledBBox(MCVector2dFR pos, MCFloat angle, MCFloat w2, MCFloat h2);
    inline void render(MCCamera * pCamera, MCVector3dFR pos, MCFloat angle);
    inline void renderScaled(MCCamera * pCamera, MCVector3dFR pos, MCFloat wr, MCFloat hr, MCFloat angle);
    inline void renderShadow(MCCamera * pCamera, MCVector2dFR pos, MCFloat angle);
    inline void renderShadowScaled(MCCamera * pCamera, MCVector2dFR pos, MCFloat wr, MCFloat hr, MCFloat angle);
    inline void doAlphaTest() const;
    inline void doAlphaBlend() const;

    GLuint m_handle;
    MCFloat m_w;
    MCFloat m_w2;
    MCFloat m_h;
    MCFloat m_h2;
    MCVector2dF m_center;
    bool m_centerSet;
    bool m_useAlphaTest;
    GLenum m_alphaFunc;
    GLclampf m_alphaThreshold;
    bool m_useAlphaBlend;
    GLenum m_src;
    GLenum m_dst;
    friend class MCSurface;
};

#endif // MCSURFACEIMPL_HH
