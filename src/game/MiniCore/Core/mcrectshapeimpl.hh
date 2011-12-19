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

#ifndef MCRECTSHAPEIMPL_HH
#define MCRECTSHAPEIMPL_HH

#include "mcshapeimpl.hh"
#include "mcedge.hh"
#include "mcobbox.hh"

class MCObject;
class MCCamera;

//! Implementation class of MCRectShape.
class MCRectShapeImpl : public MCShapeImpl
{
private:
  MCRectShapeImpl(MCObject * pParent, MCFloat width, MCFloat height);
  virtual ~MCRectShapeImpl();
  static MCUint m_typeID;
  inline MCEdge<MCFloat> edgeForPoint(const MCVector2d<MCFloat> & p) const;
  inline int interpenetrationDepth(
      const MCVector2d<MCFloat> & p1, const MCVector2d<MCFloat> & p2) const;
  inline MCVector2d<MCFloat> contactNormal(const MCVector2d<MCFloat> & p) const;
  void renderShapeOutline(MCCamera * pCamera);
  MCOBBox<MCFloat> m_obbox;
  friend class MCRectShape;
};

#endif // MCRECTSHAPEIMPL_HH
