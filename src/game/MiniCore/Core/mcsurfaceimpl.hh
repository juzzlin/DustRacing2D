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
public:

  MCSurfaceImpl(GLuint handle, MCFloat width, MCFloat height, bool useAlphaTest);
  MCSurfaceImpl(GLuint handle, MCFloat width, MCFloat height,
                const MCVector2d<MCFloat> & center, bool useAlphaTest);
  virtual ~MCSurfaceImpl() {};

  MCBBox<MCFloat> inline rotatedBBox(MCFloat x, MCFloat y, int angle);
  MCBBox<MCFloat> inline rotatedScaledBBox(MCFloat x, MCFloat y, int angle, MCFloat w2, MCFloat h2);
  inline void render(MCCamera * pCamera, MCFloat x, MCFloat y, MCFloat z, int angle);
  inline void renderScaled(MCCamera * pCamera, MCFloat x, MCFloat y, MCFloat z, MCFloat wr, MCFloat hr, int angle);
  inline void renderShadow(MCCamera * pCamera, MCFloat x, MCFloat y, int angle);
  inline void renderShadowScaled(MCCamera * pCamera, MCFloat x, MCFloat y, MCFloat wr, MCFloat hr, int angle);

private:

  GLuint m_handle;
  MCFloat m_w;
  MCFloat m_w2;
  MCFloat m_h;
  MCFloat m_h2;
  MCVector2d<MCFloat> m_center;
  bool m_centerSet;
  bool m_useAlphaTest;
  friend class MCSurface;
};

#endif // MCSURFACEIMPL_HH
