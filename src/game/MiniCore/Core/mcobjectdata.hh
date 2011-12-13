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

#ifndef MCOBJECTDATA_HH
#define MCOBJECTDATA_HH

#include "mcmacros.hh"
#include "mctypes.hh"
#include <string>

class MCObjectDataImpl;

//! Base class for object data classes.
class MCObjectData
{
public:

    //! Constructor.
    explicit MCObjectData(const std::string & typeId);

    //! Destructor.
    virtual ~MCObjectData();

    //! Get type id.
    std::string typeId() const;

    //! Set mass.
    void setMass(MCFloat newMass);

    //! Get mass.
    MCFloat mass() const;

private:

    DISABLE_COPY(MCObjectData);
    DISABLE_ASSI(MCObjectData);
    MCObjectDataImpl * const m_pImpl;
    friend class MCObjectDataImpl;
};

#endif // MCOBJECTDATA_HH
