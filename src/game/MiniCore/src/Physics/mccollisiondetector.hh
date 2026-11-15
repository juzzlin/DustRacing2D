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

#ifndef MCCOLLISIONDETECTOR_HH
#define MCCOLLISIONDETECTOR_HH

#include "mcmacros.hh"
#include "mcvector2d.hh"

#include <map>
#include <set>
#include <vector>

class MCCircleShape;
class MCObject;
class MCObjectGrid;
class MCRectShape;

//! Collision detector and contact generator.
class MCCollisionDetector
{
public:
    //! Constructor.
    MCCollisionDetector();

    //! Destructor.
    virtual ~MCCollisionDetector()
    {
    }

    void clear();

    void remove(MCObject & object);

    //! Detect collisions and generate contacts. Contacts are stored to MCObject.
    unsigned int detectCollisions(MCObjectGrid & objectGrid);

    //! Iterate current collisions and generate contacts. Contacts are stored to MCObject.
    unsigned int iterateCurrentCollisions();

private:
    DISABLE_COPY(MCCollisionDetector);
    DISABLE_ASSI(MCCollisionDetector);

    bool areCurrentlyColliding(MCObject & object1, MCObject & object2);

    bool processPossibleCollision(MCObject & object1, MCObject & object2);

    //! Contact points between two shapes.
    struct Manifold
    {
        struct Point
        {
            MCVector2dF location;

            float depth = 0;
        };

        //! Normal that pushes the second shape out of the first one.
        MCVector2dF normal;

        //! One or two points. Empty if the shapes don't overlap.
        std::vector<Point> points;
    };

    //! Separating axis test of two oriented boxes. Contact points are found by clipping
    //! the incident edge of one box against the reference face of the other one.
    bool testRectAgainstRect(MCRectShape & rect1, MCRectShape & rect2);

    //! Exact test using the point of the box closest to the center of the circle.
    bool testRectAgainstCircle(MCRectShape & rect, MCCircleShape & circle);

    bool testCircleAgainstCircle(MCCircleShape & circle1, MCCircleShape & circle2);

    //! Sends the collision events and stores the contacts to both objects.
    //! \return true if the objects collide.
    bool processManifold(MCObject & object1, MCObject & object2, const Manifold & manifold);

    using CollisionMap = std::map<MCObject *, std::set<MCObject *>>;
    CollisionMap m_currentCollisions;
};

#endif // MCCOLLISIONDETECTOR_HH
