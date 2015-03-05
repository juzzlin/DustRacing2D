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

#include "mctypes.hh"
#include "mcmacros.hh"
#include "mcbbox.hh"
#include "mcglcolor.hh"
#include "mcglobjectbase.hh"
#include "mcglmaterial.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <cmath>
#include <string>
#include <vector>

using std::string;

class  MCCamera;
struct MCGLTexCoord;
class  MCGLVertex;

/** Renderable 3d mesh object. */
class MCMesh : public MCGLObjectBase
{
public:

    struct Face
    {
        struct Vertex
        {
            float x, y, z; // Location
            float u, v;    // Texture coordinates
            float i, j, k; // Normal
        };

        std::vector<Vertex> vertices;
    };

    typedef std::vector<Face> FaceVector;

    //! Constructor.
    explicit MCMesh(const FaceVector & faces, MCGLMaterialPtr material);

    //! Destructor.
    virtual ~MCMesh() {};

    //! Set color.
    void setColor(const MCGLColor & color);

    //! Set scaling factors.
    void setScale(MCFloat x, MCFloat y, MCFloat z);

    //! Set scaling by dimensions.
    void setScale(MCFloat w, MCFloat h);

    /*! Render by using the default size.
     * \param pos The position.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     */
    void render(MCCamera * camera, MCVector3dFR pos, MCFloat angle, bool autoBind = true);

    //! Render (fake) shadow
    void renderShadow(MCCamera * camera, MCVector2dFR pos, MCFloat angle, bool autoBind = true);

    //! Render the vertex buffer only. bind() must be called separately.
    void render();

    //! Get width
    MCFloat width() const;

    //! Get height
    MCFloat height() const;

    //! Get minimum Z
    MCFloat minZ() const;

    //! Get maximum Z
    MCFloat maxZ() const;

private:

    void init(const FaceVector & faces);

    void initVBOs(
        const MCGLVertex   * vertices,
        const MCGLVertex   * normals,
        const MCGLTexCoord * texCoords,
        const GLfloat      * colors);

    int       m_numVertices;
    MCFloat   m_w, m_h;
    MCFloat   m_minZ, m_maxZ;
    MCGLColor m_color;
    MCFloat   m_sx, m_sy, m_sz;
};

#endif // MCMESH_HH
