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

//! Private class of MCObjectData.
class MCObjectDataImpl
{
private:
    MCObjectDataImpl();
    std::string typeId;
    MCFloat mass;
    MCObjectData::Shape shape;
    MCFloat shapeRadius;
    MCFloat shapeWidth;
    MCFloat shapeHeight;
    MCFloat restitution;
    MCFloat xyFriction;
    bool stationary;
    int layer;
    friend class MCObjectData;
};

MCObjectDataImpl::MCObjectDataImpl()
: typeId("")
, mass(0.0f)
, shape(MCObjectData::None)
, shapeRadius(0.0f)
, shapeWidth(0.0f)
, shapeHeight(0.0f)
, restitution(0.5f)
, xyFriction(0.0f)
, stationary(false)
, layer(0)
{
}

MCObjectData::MCObjectData(const std::string & typeId)
: m_pImpl(new MCObjectDataImpl)
{
    m_pImpl->typeId = typeId;
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
    m_pImpl->shapeWidth  = width;
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

void MCObjectData::setRestitution(MCFloat restitution)
{
    m_pImpl->restitution = restitution;
}

MCFloat MCObjectData::restitution() const
{
    return m_pImpl->restitution;
}

void MCObjectData::setXYFriction(MCFloat friction)
{
    m_pImpl->xyFriction = friction;
}

MCFloat MCObjectData::xyFriction() const
{
    return m_pImpl->xyFriction;
}

void MCObjectData::setStationary(bool stationary)
{
    m_pImpl->stationary = stationary;
}

bool MCObjectData::stationary() const
{
    return m_pImpl->stationary;
}

void MCObjectData::setLayer(int layer)
{
    m_pImpl->layer = layer;
}

int MCObjectData::layer() const
{
    return m_pImpl->layer;
}

MCObjectData::~MCObjectData()
{
    delete m_pImpl;
}
