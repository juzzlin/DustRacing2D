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

#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include "objectbase.hpp"

#include <vector>
#include <memory>

//! Container for all objects.
class Objects
{
public:

    typedef std::vector<ObjectBase *> ObjectVector;

    //! Constructor.
    Objects();

    //! Add an object.
    void add(ObjectBase & object, bool takeOwnership = false);

    //! Remove an object.
    void remove(ObjectBase & object);

    //! Return object count.
    unsigned int count() const;

    //! Return given object.
    ObjectBase & object(unsigned int index) const;

private:

    ObjectVector m_objects;
    std::vector<std::shared_ptr<ObjectBase> > m_owned;
};

#endif // OBJECTS_HPP
