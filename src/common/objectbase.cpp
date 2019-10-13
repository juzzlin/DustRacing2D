// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "objectbase.hpp"

ObjectBase::ObjectBase(QString category, QString role)
  : m_category(category)
  , m_role(role)
  , m_forceStationary(false)
{
}

ObjectBase::~ObjectBase()
{
}

QPointF ObjectBase::location() const
{
    return m_location;
}

void ObjectBase::setLocation(QPointF newLocation)
{
    m_location = newLocation;
}

QString ObjectBase::category() const
{
    return m_category;
}

QString ObjectBase::role() const
{
    return m_role;
}

bool ObjectBase::forceStationary() const
{
    return m_forceStationary;
}

void ObjectBase::setForceStationary(bool forceStationary)
{
    m_forceStationary = forceStationary;
}
