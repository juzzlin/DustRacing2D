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

#ifndef MCQUADTREE_HH
#define MCQUADTREE_HH

#include "mcbbox.hh"
#include "mcmacros.hh"
#include "mcobject.hh"

#include <QVector>
#include <QSet>

class MCQuadtreeImpl;

/*! A geometrically structured MCQuadtree used for fast collision detection.
 *  The tree stores objects inherited from MCObject -class.
 *  A (2d) collision test for a given object can be requested against all 
 *  objects of a given typeid.
 */
class MCQuadtree
{
public:

  typedef QSet<MCObject *> ObjectSet;

  /*! Constructor.
   * \param x1,y1,x2,y2 represent the size of the first-level bounding box
   * \param leafMaxW,leafMaxH are the maximum dimensions for leaves
   */
  MCQuadtree(MCFloat x1, MCFloat y1, MCFloat x2, MCFloat y2, UINT leafMaxW, UINT leafMaxH);

  /*! Insert an object into the tree (O(log(N))
   *  \param p is the object to be inserted
   */
  void insert(MCObject * p);

  /*! Remove an object from the tree (O(log(N))
   *  \param p is the object to be removed. Does nothing if p is invalid.
   *  \return true if was removed.
   */
  bool remove(MCObject * p);

  //! Remove all objects.
  void removeAll();

  //! Get objects within given distance and of given type
  void getObjectsWithinDistance(MCFloat x, MCFloat y, MCFloat d, ObjectSet & resultObjs,
                                UINT typeId = 0);

  //! Get all objects of given type overlapping given BBox
  void getObjectsWithinBBox(const MCBBox<MCFloat> & rBBox, ObjectSet & resultObjs,
                            UINT typeId = 0);

  /*! Get bbox collisions involving the given object
   * \param p Object to be tested
   * \param rVectPObjs Vector in which colliding object are stored
   * \param typeId Match MCObjects of given type only
   */
  void getBBoxCollisions(const MCObject * p, ObjectSet & resultObjs,
                         UINT typeId = 0);

  //! Get bounding box 
  const MCBBox<MCFloat> & bbox() const;

private:

  DISABLE_COPY(MCQuadtree);
  DISABLE_ASSI(MCQuadtree);
  shared_ptr<MCQuadtreeImpl> const m_pImpl;
  friend class MCQuadtreeImpl;
};

#endif // MCQUADTREE_HH

