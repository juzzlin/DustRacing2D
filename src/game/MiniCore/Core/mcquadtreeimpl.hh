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

#ifndef MCQUADTREEIMPL_HH
#define MCQUADTREEIMPL_HH

#include <QVector>

//! Implementation class for MCQuadtree
class MCQuadtreeImpl
{
public:

  MCQuadtreeImpl(MCQuadtree * pPublic, MCFloat x1, MCFloat y1, MCFloat x2, MCFloat y2,
                 UINT leafMaxW, UINT leafMaxH);
  ~MCQuadtreeImpl();

private:

  void removeAll();
  void build();
  inline void insert(MCObject * p);
  inline bool remove(MCObject * p);
  inline void getIndexRange(const MCBBox<MCFloat> & bbox);
  inline void getBBoxCollisions(const MCObject * p, MCQuadtree::ObjectSet & resultObjs, UINT typeId);
  inline void getObjectsWithinDistance(MCFloat x, MCFloat y, MCFloat d, MCQuadtree::ObjectSet & resultObjs,
                                       UINT typeId);
  inline void getObjectsWithinBBox(const MCBBox<MCFloat> & rBBox, MCQuadtree::ObjectSet & resultObjs,
                                   UINT typeId);
  MCQuadtree * const m_pPublic;
  MCBBox<MCFloat> m_bbox;
  UINT m_leafMaxW, m_leafMaxH, m_horSize, m_verSize;
  UINT m_i0, m_i1, m_j0, m_j1;
  MCFloat m_helpHor;
  MCFloat m_helpVer;
  MCQuadtree::ObjectSet * m_matrix;
  friend class MCQuadtree;
};

#endif // MCQUADTREEIMPL_HH
