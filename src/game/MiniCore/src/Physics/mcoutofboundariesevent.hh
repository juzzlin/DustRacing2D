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

#ifndef MCOUTOFBOUNDARIESEVENT_HH
#define MCOUTOFBOUNDARIESEVENT_HH

#include "mcevent.hh"

class MCObject;

/*! \class MCOutOfBoundariesEvent
 *  \brief Event sent when an objects goes out of world boundaries.
 */
class MCOutOfBoundariesEvent : public MCEvent
{
public:

    //! Edge that was violated
    enum ViolatedEdge {West, East, North, South, Top, Bottom};

    /*! Constructor
     * \param edge The edge that has been violated by an object
     */
    explicit MCOutOfBoundariesEvent(ViolatedEdge edge, MCObject & violatingObject);

    //! Destructor
    ~MCOutOfBoundariesEvent();

    //! Get violated edge
    ViolatedEdge violatedEdge() const;

    //! Return the typeID
    static MCUint typeID();

    //! \reimp
    virtual MCUint instanceTypeID() const;

    MCObject & violatingObject() const;

private:

    DISABLE_COPY(MCOutOfBoundariesEvent);
    DISABLE_ASSI(MCOutOfBoundariesEvent);

    MCOutOfBoundariesEvent::ViolatedEdge m_violatedEdge;

    MCObject & m_violatingObject;

    static MCUint m_typeID;
};

#endif // MCOUTOFBOUNDARIESEVENT_HH
