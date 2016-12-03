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

#ifndef MCCOLLISIONEVENT_HH
#define MCCOLLISIONEVENT_HH

#include "mcevent.hh"
#include "mcvector3d.hh"

class MCObject;

/*! \class MCCollisionEvent
 *  \brief Event sent when two objects collides.
 *
 *  The receiving object must accept the event in their
 *  event handlers (event() or collisionEvent()) in order to
 *  trigger collision physics. This way objects can filter
 *  with which objects they want to collide.
 */
class MCCollisionEvent : public MCEvent
{
public:

    /*! Constructor.
     * \param collidingObject The colliding object. */
    MCCollisionEvent(MCObject & collidingObject, MCVector3dF contactPoint, bool isPrimary = true);

    //! Destructor.
    ~MCCollisionEvent();

    //! Get colling object.
    MCObject & collidingObject() const;

    //! Get the contact point.
    const MCVector3dF & contactPoint() const;

    //! Return the typeID.
    static MCUint typeID();

    //! \reimp
    virtual MCUint instanceTypeID() const;

    /*! \return true if the event is a primary event. Non-primary events may be generated
     *  during collision resolution. */
    bool isPrimary() const;

private:

    DISABLE_COPY(MCCollisionEvent);
    DISABLE_ASSI(MCCollisionEvent);

    MCObject & m_collidingObject;

    MCVector3dF m_contactPoint;

    bool m_isPrimary;

    static MCUint m_typeID;
};

#endif // MCCOLLISIONEVENT_HH
