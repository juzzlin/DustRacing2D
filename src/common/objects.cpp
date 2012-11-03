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

#include "objects.hpp"

#include <algorithm>

Objects::Objects()
{
}

void Objects::add(ObjectBase & object, bool takeOwnership)
{
    if (find(m_objects.begin(), m_objects.end(), &object) ==
        m_objects.end())
    {
        m_objects.push_back(&object);

        if (takeOwnership)
        {
            m_owned.push_back(std::shared_ptr<ObjectBase>(&object));
        }
    }
}

void Objects::remove(ObjectBase & object)
{
    auto i = find(m_objects.begin(), m_objects.end(), &object);
    if (i != m_objects.end())
    {
        m_objects.erase(i);
    }
}

unsigned int Objects::count() const
{
    return m_objects.size();
}

ObjectBase & Objects::object(unsigned int index) const
{
    return *m_objects.at(index);
}
