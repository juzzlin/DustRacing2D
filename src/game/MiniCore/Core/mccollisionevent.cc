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

#include "mcobject.hh"
#include "mccollisionevent.hh"
#include "mccollisioneventimpl.hh"

UINT MCCollisionEvent::m_typeID = MCEvent::registerType();

MCCollisionEventImpl::MCCollisionEventImpl(MCObject * p) :
    m_pCollidingObject(p)
{}

MCCollisionEvent::MCCollisionEvent(MCObject * p) :
    m_pImpl(new MCCollisionEventImpl(p))
{}

UINT MCCollisionEvent::typeID()
{
  return MCCollisionEvent::m_typeID;
}

UINT MCCollisionEvent::instanceTypeID() const
{
  return MCCollisionEvent::m_typeID;
}

MCObject * MCCollisionEvent::collidingObject() const
{
  return m_pImpl->m_pCollidingObject;
}

MCCollisionEvent::~MCCollisionEvent()
{
  delete m_pImpl;
}

