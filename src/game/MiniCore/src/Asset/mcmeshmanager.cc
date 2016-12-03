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
#include "mcglmaterial.hh"
#include "mctypes.hh"
#include "mcmesh.hh"
#include "mcmeshconfigloader.hh"
#include "mcmeshloader.hh"
#include "mcsurface.hh"

#include <QDir>
#include <QString>

#include <cassert>
#include <exception>

MCMeshManager::MCMeshManager()
{
}

MCMesh & MCMeshManager::createMesh(
    const MCMeshMetaData & data, const MCMesh::FaceVector & faces)
{
    // Create material
    MCGLMaterialPtr material(new MCGLMaterial);

    material->setTexture(
        data.texture1 != "" ?
        MCAssetManager::surfaceManager().surface(data.texture1).material()->texture(0) : 0,
        0);

    material->setTexture(
        data.texture2 != "" ?
        MCAssetManager::surfaceManager().surface(data.texture2).material()->texture(0) : 0,
        1);

    // Create a new MCMesh object
    MeshPtr mesh(new MCMesh(faces, material));

    m_meshMap[data.handle] = mesh;

    return *mesh;
}

void MCMeshManager::load(
    const std::string & configFilePath, const std::string & baseDataPath)
{
    MCMeshConfigLoader configLoader;
    MCMeshLoader       modelLoader;

    if (configLoader.load(configFilePath))
    {
        for (unsigned int i = 0; i < configLoader.meshCount(); i++)
        {
            const MCMeshMetaData & metaData = configLoader.mesh(i);

            QString modelPath =
                QString(baseDataPath.c_str()) + QDir::separator().toLatin1() + metaData.modelPath.c_str();
            modelPath.replace("./", "");
            modelPath.replace("//", "/");

            if (modelLoader.load(modelPath))
            {
                createMesh(metaData, modelLoader.faces());
            }
            else
            {
                throw std::runtime_error("Loading mesh '" + modelPath.toStdString() + "' failed!");
            }
        }
    }
    else
    {
        throw std::runtime_error("Parsing '" + configFilePath + "' failed!");
    }
}

MCMesh & MCMeshManager::mesh(const std::string & handle) const
{
    // Try to find existing mesh for the handle
    if (m_meshMap.count(handle) == 0)
    {
        throw std::runtime_error("Cannot find mesh object for handle '" + handle + "'");
    }

    // Yes: return handle for the mesh
    MeshPtr mesh = m_meshMap.find(handle)->second;
    assert(mesh);
    return *mesh;
}

