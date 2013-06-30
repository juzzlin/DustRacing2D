// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcmeshmanager.hh"

#include "mcassetmanager.hh"
#include "mctypes.hh"
#include "mcmesh.hh"
#include "mcmeshconfigloader.hh"
#include "mcmeshloader.hh"
#include "mcsurface.hh"

#include <QDir>
#include <cassert>

MCMeshManager::MCMeshManager()
{
}

MCMesh & MCMeshManager::createMesh(
    const MCMeshMetaData & data, const MCMesh::FaceVector & faces) throw (MCException)
{
    // Create a new MCMesh object
    MeshPtr mesh(
        new MCMesh(
            faces,
            data.texture1 != "" ?
                MCAssetManager::surfaceManager().surface(data.texture1).texture1() : 0,
            data.texture2 != "" ?
                MCAssetManager::surfaceManager().surface(data.texture2).texture2() : 0));

    m_meshMap[data.handle] = mesh;

    return *mesh;
}

void MCMeshManager::load(
    const std::string & configFilePath, const std::string & baseDataPath) throw (MCException)
{
    MCMeshConfigLoader configLoader;
    MCMeshLoader       modelLoader;

    if (configLoader.load(configFilePath))
    {
        for (unsigned int i = 0; i < configLoader.meshCount(); i++)
        {
            const MCMeshMetaData & metaData = configLoader.mesh(i);

            const std::string modelPath =
                baseDataPath + QDir::separator().toLatin1() + metaData.modelPath;

            if (modelLoader.load(modelPath))
            {
                createMesh(metaData, modelLoader.faces());
            }
            else
            {
                throw MCException("Loading mesh '" + modelPath + "' failed!");
            }
        }
    }
    else
    {
        throw MCException("Parsing '" + configFilePath + "' failed!");
    }
}

MCMesh & MCMeshManager::mesh(const std::string & handle) const throw (MCException)
{
    // Try to find existing mesh for the handle
    if (m_meshMap.count(handle) == 0)
    {
        throw MCException("Cannot find mesh object for handle '" + handle + "'");
    }

    // Yes: return handle for the mesh
    MeshPtr mesh = m_meshMap.find(handle)->second;
    assert(mesh);
    return *mesh;
}

