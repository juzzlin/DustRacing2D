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

#include "mccollisionevent.hh"
#include "mcobject.hh"

unsigned int MCCollisionEvent::m_typeId = MCEvent::registerType();

MCCollisionEvent::MCCollisionEvent(MCObject & collidingObject, MCVector3dF contactPoint)
  : m_collidingObject(collidingObject)
  , m_contactPoint(contactPoint)
{
}

unsigned int MCCollisionEvent::typeId()
{
    return MCCollisionEvent::m_typeId;
}

unsigned int MCCollisionEvent::instanceTypeId() const
{
    return MCCollisionEvent::m_typeId;
}

MCObject & MCCollisionEvent::collidingObject() const
{
    return m_collidingObject;
}

const MCVector3dF & MCCollisionEvent::contactPoint() const
{
    return m_contactPoint;
}

MCCollisionEvent::~MCCollisionEvent()
{
}
