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

#ifndef MCMESH_HH
#define MCMESH_HH

#include <MCGLEW>

#include "mcbbox.hh"
#include "mcglcolor.hh"
#include "mcglmaterial.hh"
#include "mcglobjectbase.hh"
#include "mcmacros.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <cmath>
#include <memory>
#include <string>
#include <vector>

using std::string;

class MCCamera;
struct MCGLTexCoord;
class MCGLVertex;

/** Renderable 3d mesh object. */
class MCMesh : public MCGLObjectBase
{
public:
    struct Face
    {
        struct Vertex
        {
            float x = 0, y = 0, z = 0; // Location
            float u = 0, v = 0; // Texture coordinates
            float i = 0, j = 0, k = 0; // Normal
        };

        std::vector<Vertex> vertices;
    };

    typedef std::vector<Face> FaceVector;

    //! Constructor.
    MCMesh(std::string handle, const FaceVector & faces, MCGLMaterialPtr material);

    //! Destructor.
    virtual ~MCMesh() override = default;

    /*! Render by using the default size.
     * \param pos The position.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     */
    virtual void render(MCCamera * camera, MCVector3dFR pos, float angle) override;

    //! Render (fake) shadow
    virtual void renderShadow(MCCamera * camera, MCVector3dFR pos, float angle) override;

private:
    void init(const FaceVector & faces);

    void initVBOs();
};

using MCMeshPtr = std::shared_ptr<MCMesh>;

#endif // MCMESH_HH
