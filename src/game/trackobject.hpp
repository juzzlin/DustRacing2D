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

#ifndef TRACKOBJECT_HPP
#define TRACKOBJECT_HPP

#include "../common/objectbase.hpp"
#include <memory>

#include <MCObject>

//! TrackObject is a container class used in TrackData.
class TrackObject : public ObjectBase
{
public:
    //! Constructor.
    TrackObject(QString category, QString role, MCObjectPtr object);

    //! \return reference to the associated MCObject.
    MCObject & object() const;

private:
    //! The associated MCObject.
    MCObjectPtr m_object;
};

#endif // TRACKOBJECT_HPP
