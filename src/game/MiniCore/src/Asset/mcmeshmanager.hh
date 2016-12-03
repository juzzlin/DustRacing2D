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

#ifndef MCMESHMANAGER_HH
#define MCMESHMANAGER_HH

#include <memory>
#include <string>
#include <unordered_map>

#include "mcmacros.hh"
#include "mcmesh.hh"
#include "mcmeshmetadata.hh"

/*! Mesh manager base class.
 *
 * It loads model data (only .obj supported) listed in a special mapping
 * file and bulds MCMesh objects.
 *
 * MCMesh objects can be accessed via handles specified in the XML-based mapping file.
 *
 * Surfaces need to be loaded first, if textures for the meshes are defined.
 *
 * Example mapping file:
 *
 * <?xml version="1.0"?>
 *
 * <!-- Mesh config used by the game.
 *      Maps handles used in the game to .obj files. -->
 *
 * <meshes baseModelPath="./models/">
 *   <mesh handle="crate" model="cube.obj" texture1="crate"/>
 * </meshes>
 */
class MCMeshManager
{
public:

    //! Constructor.
    MCMeshManager();

    //! Destructor
    virtual ~MCMeshManager() {};

    //! Loads mesh config from strBasePath using the given mapping file strFile.
    //! \param configFilePath Path to the XML-based input file.
    //! \param baseDataPath The absolute search path for an mesh is
    //! baseDataPath + baseModelPath + fileName. baseModelPath and the fileName are
    //! defined in the input file.
    virtual void load(
        const std::string & configFilePath, const std::string & baseDataPath);

    //! Returns a mesh object associated with given strId.
    //! MCMeshManager will keep the ownership.
    //! \param handle Handle defined in the mesh config file.
    //! \return Reference to the corresponding MCMesh.
    //! \throws std::runtime_error on failure.
    MCMesh & mesh(
        const std::string & handle) const;

    //! Create a mesh from given meta data and face vector.
    MCMesh & createMesh(
        const MCMeshMetaData & data, const MCMesh::FaceVector & faces);

private:

    //! Map for resulting mesh objects
    typedef std::shared_ptr<MCMesh> MeshPtr;
    typedef std::unordered_map<std::string, MeshPtr> MeshHash;
    MeshHash m_meshMap;

    DISABLE_COPY(MCMeshManager);
    DISABLE_ASSI(MCMeshManager);
};

#endif // MCMESHMANAGER_HH
