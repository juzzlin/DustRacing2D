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

#include "../mcmacros.hh"
#include "../mctypes.hh"

#include <vector>

class MCCollisionDetectorImpl;
class MCObject;
class MCObjectTree;

//! Contact resolver / collision detector
class MCCollisionDetector
{
public:
    //! Constructor
    MCCollisionDetector();

    //! Destructor
    virtual ~MCCollisionDetector();

    //! Detect collisions and generate contacts. Contacts are stored
    //! to MCObject.
    MCUint detectCollisions(
        std::vector<MCObject *> & objs, MCObjectTree & objectTree);

    //! Turn collision events on/off.
    void enableCollisionEvents(bool enable);

private:

    /*! Process a suspected collision.
     * \param object1 1st object.
     * \param object2 2nd object.
     * \return true if objects really collided.
     */
    bool processPossibleCollision(MCObject & object1, MCObject & object2);

    DISABLE_COPY(MCCollisionDetector);
    DISABLE_ASSI(MCCollisionDetector);
    MCCollisionDetectorImpl * const m_pImpl;
    friend class MCCollisionDetectorImpl;
};

#endif // MCCOLLISIONDETECTOR_HH
