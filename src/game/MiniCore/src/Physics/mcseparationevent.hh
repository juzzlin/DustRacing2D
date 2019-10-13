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

#ifndef MCSEPARATIONEVENT_HH
#define MCSEPARATIONEVENT_HH

#include "mcevent.hh"
#include "mcvector3d.hh"

class MCObject;

/*! \class MCSeparationEvent
 *  \brief Event sent when two objects does not collide anymore.*/
class MCSeparationEvent : public MCEvent
{
public:
    /*! Constructor.
     * \param separatedObject The separated object. */
    MCSeparationEvent(MCObject & separatedObject);

    //! Destructor.
    ~MCSeparationEvent() override;

    //! Get separated object.
    MCObject & separatedObject() const;

    //! Return the typeId.
    static unsigned int typeId();

    //! \reimp
    virtual unsigned int instanceTypeId() const override;

private:
    DISABLE_COPY(MCSeparationEvent);
    DISABLE_ASSI(MCSeparationEvent);

    MCObject & m_separatedObject;

    static unsigned int m_typeId;
};

#endif // MCSEPARATIONEVENT_HH
