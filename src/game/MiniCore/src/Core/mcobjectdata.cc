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

MCObjectData::MCObjectData(const std::string & typeId)
  : m_typeId(typeId)
  , m_mass(0.0)
  , m_shape(MCObjectData::Default)
  , m_shapeRadius(0.0)
  , m_shapeWidth(0.0)
  , m_shapeHeight(0.0)
  , m_restitution(0.5)
  , m_xyFriction(0.0)
  , m_stationary(false)
  , m_angle(0)
{
}

std::string MCObjectData::typeId() const
{
    return m_typeId;
}

void MCObjectData::setMass(float newMass)
{
    m_mass = newMass;
}

float MCObjectData::mass() const
{
    return m_mass;
}

void MCObjectData::setShapeRadius(float radius)
{
    m_shape = MCObjectData::Circle;
    m_shapeRadius = radius;
}

float MCObjectData::shapeRadius() const
{
    return m_shapeRadius;
}

void MCObjectData::setShapeWidth(float width)
{
    m_shape = MCObjectData::Rect;
    m_shapeWidth = width;
}

float MCObjectData::shapeWidth() const
{
    return m_shapeWidth;
}

void MCObjectData::setShapeHeight(float height)
{
    m_shape = MCObjectData::Rect;
    m_shapeHeight = height;
}

float MCObjectData::shapeHeight() const
{
    return m_shapeHeight;
}

MCObjectData::Shape MCObjectData::shape() const
{
    return m_shape;
}

void MCObjectData::setRestitution(float restitution)
{
    m_restitution = restitution;
}

float MCObjectData::restitution() const
{
    return m_restitution;
}

void MCObjectData::setXYFriction(float friction)
{
    m_xyFriction = friction;
}

float MCObjectData::xyFriction() const
{
    return m_xyFriction;
}

void MCObjectData::setIsStationary(bool stationary)
{
    m_stationary = stationary;
}

bool MCObjectData::stationary() const
{
    return m_stationary;
}

void MCObjectData::setInitialLocation(MCVector3dF location)
{
    m_location = location;
}

MCVector3dF MCObjectData::initialLocation() const
{
    return m_location;
}

void MCObjectData::setInitialAngle(int angle)
{
    m_angle = angle;
}

int MCObjectData::initialAngle() const
{
    return m_angle;
}
