// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcobjectdata.hh"
#include "mcobjectdataimpl.hh"

MCObjectData::MCObjectData(const std::string & typeId)
: m_pImpl(new MCObjectDataImpl)
{
    m_pImpl->typeId = typeId;
}

MCObjectDataImpl::MCObjectDataImpl()
: typeId("")
, mass(0.0f)
, shape(MCObjectData::Circle)
, shapeRadius(0.0f)
, shapeWidth(0.0f)
, shapeHeight(0.0f)
{
}

std::string MCObjectData::typeId() const
{
    return m_pImpl->typeId;
}

void MCObjectData::setMass(MCFloat newMass)
{
    m_pImpl->mass = newMass;
}

MCFloat MCObjectData::mass() const
{
    return m_pImpl->mass;
}

void MCObjectData::setShapeRadius(MCFloat radius)
{
    m_pImpl->shape       = MCObjectData::Circle;
    m_pImpl->shapeRadius = radius;
}

MCFloat MCObjectData::shapeRadius() const
{
    return m_pImpl->shapeRadius;
}

void MCObjectData::setShapeWidth(MCFloat width)
{
    m_pImpl->shape       = MCObjectData::Rect;
    m_pImpl->shapeHeight = width;
}

MCFloat MCObjectData::shapeWidth() const
{
    return m_pImpl->shapeWidth;
}

void MCObjectData::setShapeHeight(MCFloat height)
{
    m_pImpl->shape       = MCObjectData::Rect;
    m_pImpl->shapeHeight = height;
}

MCFloat MCObjectData::shapeHeight() const
{
    return m_pImpl->shapeHeight;
}

MCObjectData::Shape MCObjectData::shape() const
{
    return m_pImpl->shape;
}

MCObjectData::~MCObjectData()
{
    delete m_pImpl;
}
