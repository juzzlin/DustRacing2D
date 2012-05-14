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

#include "mccollisiondetector.hh"
#include "mccollisiondetectorimpl.hh"
#include "mccontact.hh"
#include "../mcobject.hh"
#include "../mcsegment.hh"
#include "../mcshape.hh"
#include "../mcrectshape.hh"
#include "../mccollisionevent.hh"

#include <cassert>

MCCollisionDetectorImpl::MCCollisionDetectorImpl()
{}

MCCollisionDetectorImpl::~MCCollisionDetectorImpl()
{}

MCCollisionDetector::MCCollisionDetector() :
    m_pImpl(new MCCollisionDetectorImpl)
{}

MCCollisionDetector::~MCCollisionDetector()
{
    delete m_pImpl;
}

bool MCCollisionDetectorImpl::processRectRect(
    MCRectShape & shape1, MCRectShape & shape2)
{
    if (&shape1.parent() == &shape2.parent())
    {
        return false;
    }

    const MCOBBox<MCFloat> & obbox1(shape1.obbox());

    MCVector2dF vertex;
    MCVector2dF contactNormal;

    MCFloat depth   = 0;
    bool depthIsSet = false;
    bool collided   = false;

    // Loop thru all vertices of shape1 and generate contacts
    for (MCUint i = 0; i < 4; i++)
    {
        if (shape2.contains(obbox1.vertex(i)))
        {
            // Send collision event to owner of shape1 and generate a contact
            // if accepted.
            MCCollisionEvent ev1(shape2.parent(), obbox1.vertex(i));
            MCObject::sendEvent(shape1.parent(), ev1);

            vertex     = obbox1.vertex(i);
            depth      = 0;
            depthIsSet = false;

            if (ev1.accepted())
            {
                depth = shape2.interpenetrationDepth(
                    MCSegment<MCFloat>(vertex, shape1.location()), contactNormal);
                depthIsSet = true;

                MCContact & contact = MCContact::create();
                contact.init(shape2.parent(), vertex, contactNormal, depth);
                shape1.parent().addContact(contact);

                collided = true;
            }

            // Send collision event to owner of shape2 and generate a contact
            // if accepted.
            MCCollisionEvent ev2(shape1.parent(), obbox1.vertex(i));
            MCObject::sendEvent(shape2.parent(), ev2);

            if (ev2.accepted())
            {
                if (!depthIsSet)
                {
                    depth = shape2.interpenetrationDepth(
                        MCSegment<MCFloat>(vertex, shape1.location()), contactNormal);
                }

                MCContact & contact = MCContact::create();
                contact.init(shape1.parent(), vertex, -contactNormal, depth);
                shape2.parent().addContact(contact);
            }
        }
    }

    return collided;
}

bool MCCollisionDetector::processPossibleCollision(
    MCObject & object1, MCObject & object2)
{
    if (&object1 == &object2)
    {
        return false;
    }

    // Check that both objects contain a shape
    if (object1.shape() && object2.shape())
    {
        // Store id's
        const MCUint id1 = object1.shape()->instanceTypeID();
        const MCUint id2 = object2.shape()->instanceTypeID();

        // Both rects ?
        if (id1 == MCRectShape::typeID() && id2 == MCRectShape::typeID())
        {
            // We must test first object1 against object2 and then
            // the other way around.
            const bool shape1shape2 = m_pImpl->processRectRect(
                // Static cast because we know the types now.
                *static_cast<MCRectShape *>(object1.shape()),
                *static_cast<MCRectShape *>(object2.shape()));

            const bool shape2shape1 = m_pImpl->processRectRect(
                // Static cast because we know the types now.
                *static_cast<MCRectShape *>(object2.shape()),
                *static_cast<MCRectShape *>(object1.shape()));

            return shape1shape2 || shape2shape1;
        }
    }

    return false;
}
