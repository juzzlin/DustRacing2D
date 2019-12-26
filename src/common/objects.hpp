// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include "objectbase.hpp"

#include <memory>
#include <vector>

//! Container for all objects.
class Objects
{
public:
    typedef std::vector<ObjectBasePtr> ObjectVector;

    //! Constructor.
    Objects();

    Objects(const Objects & other) = delete;

    Objects & operator=(const Objects & other) = delete;

    //! Add an object.
    void add(ObjectBasePtr object);

    //! Remove an object.
    void remove(ObjectBase & object);

    //! Clear the container.
    void clear();

    //! Return object count.
    size_t count() const;

    //! Return given object.
    ObjectBasePtr object(size_t index) const;

    ObjectVector::iterator begin();

    ObjectVector::iterator end();

    ObjectVector::const_iterator cbegin() const;

    ObjectVector::const_iterator cend() const;

private:
    ObjectVector m_objects;
};

#endif // OBJECTS_HPP
