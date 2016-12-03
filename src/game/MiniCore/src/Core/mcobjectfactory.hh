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

#ifndef MCOBJECTFACTORY_HH
#define MCOBJECTFACTORY_HH

#include "mcsurfaceobjectdata.hh"
#include "mcmeshobjectdata.hh"
#include "mcobject.hh"
#include "mcshapeview.hh"

#include <memory>
#include <vector>

class MCAssetManager;

//! MCObjectFactory is a factory class that can be used when loading objects from file.
class MCObjectFactory
{
public:

    //! Constructor.
    MCObjectFactory(MCAssetManager & assetManager);

    //! Destructor
    virtual ~MCObjectFactory();

    /*! Build an object with a surface view. */
    MCObjectPtr build(const MCSurfaceObjectData & data);

    /*! Build an object with a mesh view. */
    MCObjectPtr build(const MCMeshObjectData & data);

    /*! Build an object with a custom view. */
    MCObjectPtr build(const MCObjectData & data, MCShapeViewPtr view);

private:

    DISABLE_COPY(MCObjectFactory);
    DISABLE_ASSI(MCObjectFactory);

    void setCommonProperties(MCObject & object, const MCObjectData & data) const;

    MCAssetManager & m_assetManager;
};

#endif // MCOBJECTFACTORY_HH
