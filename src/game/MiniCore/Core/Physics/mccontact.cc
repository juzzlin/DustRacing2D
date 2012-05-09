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

#include "mccontact.hh"
#include "mccontactimpl.hh"
#include "../mcobject.hh"
#include <cassert>

MCContactImpl::MCContactImpl()
: m_pObject(nullptr)
, m_interpenetrationDepth(0.0f)
{}

MCRecycler<MCContact> MCContactImpl::m_recycler;

MCContact::MCContact()
: m_pImpl(new MCContactImpl())
{}

void MCContact::init(MCObject & object,
    const MCVector2d<MCFloat> & newContactPoint,
    const MCVector2d<MCFloat> & newContactNormal,
    MCFloat newInterpenetrationDepth)
{
    m_pImpl->m_pObject               = &object;
    m_pImpl->m_contactPoint          = newContactPoint;
    m_pImpl->m_contactNormal         = newContactNormal;
    m_pImpl->m_interpenetrationDepth = newInterpenetrationDepth;
}

MCObject & MCContact::object() const
{
    assert(m_pImpl->m_pObject);
    return *m_pImpl->m_pObject;
}

const MCVector2d<MCFloat> & MCContact::contactPoint() const
{
    return m_pImpl->m_contactPoint;
}

const MCVector2d<MCFloat> & MCContact::contactNormal() const
{
    return m_pImpl->m_contactNormal;
}

MCFloat MCContact::interpenetrationDepth() const
{
    return m_pImpl->m_interpenetrationDepth;
}

MCContact & MCContact::create()
{
    return *MCContactImpl::m_recycler.newObject();
}

void MCContact::free()
{
    return MCContactImpl::m_recycler.freeObject(this);
}

MCContact::~MCContact()
{
    delete m_pImpl;
}
