// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCOBJECTGRID_HH
#define MCOBJECTGRID_HH

#include "mcbbox.hh"
#include "mcmacros.hh"
#include "mcobject.hh"

#include <map>
#include <set>
#include <vector>

/*! A grid used for fast collision detection.
 *  The tree stores objects inherited from MCObject -class.
 *  A (2d) collision test for a given object can be requested against all
 *  objects of a given typeid. */
class MCObjectGrid
{
public:

    typedef std::set<MCObject *> ObjectSet;
    typedef std::vector<std::pair<MCObject *, MCObject *> > CollisionVector;

    //! Container for objects.
    struct GridCell
    {
        ObjectSet m_objects;
    };

    /*! Constructor.
     *  \param x1,y1,x2,y2 represent the size of the first-level bounding box.
     *  \param leafMaxW,leafMaxH are the maximum dimensions for leaves. */
    MCObjectGrid(
        MCFloat x1, MCFloat y1, MCFloat x2, MCFloat y2,
        MCFloat leafMaxW, MCFloat leafMaxH);

    //! Destructor.
    ~MCObjectGrid();

    /*! Insert an object into the tree (O(1)).
     *  \param object is the object to be inserted. */
    void insert(MCObject & object);

    /*! Remove an object from the tree (O(1)).
     *  \param object is the object to be removed.
     *  \return true if was removed. */
    bool remove(MCObject & object);

    //! Remove all objects.
    void removeAll();

    //! Get objects within given distance.
    const ObjectSet & getObjectsWithinDistance(const MCVector2dF & p, MCFloat d);

    //! Get objects within given distance.
    const ObjectSet & getObjectsWithinDistance(MCFloat x, MCFloat y, MCFloat d);

    //! Get all objects of given type overlapping given BBox.
    const ObjectSet & getObjectsWithinBBox(const MCBBox<MCFloat> & bbox);

    /*! Get possible collisions. Collisions between sleeping objects are ignored,
     *  because that gives a huge performance boost.
     *  \return possible collisions. */
    const CollisionVector & getPossibleCollisions();

    //! Get bounding box
    const MCBBox<MCFloat> & bbox() const;

private:

    DISABLE_COPY(MCObjectGrid);
    DISABLE_ASSI(MCObjectGrid);

    void setIndexRange(const MCBBox<MCFloat> & bbox);

    void build();

    MCBBox<MCFloat> m_bbox;

    MCFloat m_leafMaxW;

    MCFloat m_leafMaxH;

    MCUint m_horSize;

    MCUint m_verSize;

    MCUint m_i0, m_i1, m_j0, m_j1;

    MCFloat m_helpHor;

    MCFloat m_helpVer;

    std::vector<GridCell *> m_matrix;

    typedef std::set<GridCell *> DirtyCellCache;
    DirtyCellCache m_dirtyCellCache;
};

#endif // MCOBJECTGRID_HH
