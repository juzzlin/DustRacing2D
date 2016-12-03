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

#ifndef MCCONTACT_HH
#define MCCONTACT_HH

#include "mcvector2d.hh"
#include "mcrecycler.hh"
#include "mcmacros.hh"

class MCObject;

/*! \class MCContact
 *  \brief MCContact is a class representing a collision contact.
 *
 * MCContact is added to object (A) to notify a contact with object(B) using
 * MCObject::addContact(MCContact * p). MCWorld then processes the contacts
 * on every world update.
 */
class MCContact
{
public:

    //! Return a new (empty) contact
    static MCContact & create();

    //! Move contact to the list of free contacts
    void free();

    /*! \brief Init the contact.
     *  \param object The contacting object
     *  \param contactPoint The point of contact
     *  \param contactNormal The contact normal pointing away from pObject
     *  \param interpenetrationDepth The depth of interpenetration
     */
    void init(MCObject & object,
        const MCVector2d<MCFloat> & contactPoint,
        const MCVector2d<MCFloat> & contactNormal,
        MCFloat interpenetrationDepth);

    //! Return the contacting object
    MCObject & object() const;

    //! Return the contact point
    const MCVector2d<MCFloat> & contactPoint() const;

    //! Return the contact normal
    const MCVector2d<MCFloat> & contactNormal() const;

    //! Return the interpenetration
    MCFloat interpenetrationDepth() const;

private:

    //! Constructor disabled: use MCContact::create()
    MCContact();

    //! Destructor disabled: use MCContact::free()
    ~MCContact();

    DISABLE_COPY(MCContact);
    DISABLE_ASSI(MCContact);

    MCObject * m_pObject;
    MCVector2d<MCFloat> m_contactPoint;
    MCVector2d<MCFloat> m_contactNormal;
    MCFloat m_interpenetrationDepth;
    static MCRecycler<MCContact> m_recycler;
    friend class MCRecycler<MCContact>;
};

#endif // MCCONTACT_HH
