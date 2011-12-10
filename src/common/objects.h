// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#ifndef OBJECTS_H
#define OBJECTS_H

#include "objectbase.h"
#include <vector>

//! Container for all objects.
class Objects
{
public:

    typedef std::vector<ObjectBase *> ObjectVector;

    //! Constructor.
    Objects();

    //! Add an object.
    void add(ObjectBase & object);

    //! Remove an object.
    void remove(ObjectBase & object);

    //! Return object count.
    unsigned int count() const;

    //! Return given object.
    ObjectBase & object(unsigned int index) const;

private:

    ObjectVector m_objects;
};

#endif // OBJECTS_H
