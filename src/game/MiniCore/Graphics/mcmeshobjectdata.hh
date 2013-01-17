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

#ifndef MCMESHOBJECTDATA_HH
#define MCMESHOBJECTDATA_HH

#include "mcsurfaceobjectdata.hh"

//! Mesh object description that can be given to MCObjectFactory.
class MCMeshObjectData : public MCSurfaceObjectData
{
public:

    //! Constructor.
    explicit MCMeshObjectData(const std::string & typeId);

    //! Destructor.
    virtual ~MCMeshObjectData();

    //! Set the mesh id.
    void setMeshId(const std::string & id);

    //! Return the mesh id.
    const std::string & meshId() const;

private:

    DISABLE_COPY(MCMeshObjectData);
    DISABLE_ASSI(MCMeshObjectData);

    std::string m_meshId;
};

#endif // MCMESHOBJECTDATA_HH
