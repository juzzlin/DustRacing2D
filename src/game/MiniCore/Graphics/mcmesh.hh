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
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <cmath>
#include <string>
#include <vector>

using std::string;

class MCCamera;
class MCGLShaderProgram;
class MCGLTexCoord;
class MCGLVertex;

/** Renderable 3d mesh object. */
class MCMesh
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

    /*! Constructor.
     *  \param handle Handle of the corresponding OpenGL texture 1.
     *  \param handle Handle of the corresponding OpenGL texture 2 (multitexturing) or zero. */
    explicit MCMesh(const FaceVector & faces, GLuint handle1 = 0, GLuint handle2 = 0);

    //! Destructor.
    virtual ~MCMesh();

    //! Set color.
    void setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a = 1.0);

    //! Set scaling factors.
    void setScale(MCFloat x, MCFloat y, MCFloat z);

    //! Set scaling by dimensions.
    void setScale(MCFloat w, MCFloat h);

    /*! Render by using the default size.
     * \param pos The position.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     */
    void render(MCCamera * camera, MCVector3dFR pos, MCFloat angle,
        bool autoBind = true);

    //! Render (fake) shadow
    void renderShadow(MCCamera * camera, MCVector2dFR pos, MCFloat angle,
        bool autoBind = true);

    //! Render the vertex buffer only. bind() must be called separately.
    void render();

    /*! Manually enable/disable OpenGL client states and texturing environment.
     *  This can be used to save some function calls when rendering the same
     *  mesh multiple times.
     *  \see render() */
    void bind(bool enable) const;

    /*! Manually enable/disable OpenGL client states and texturing environment.
     *  This can be used to save some function calls when rendering the same
     *  mesh multiple times.
     *  \see renderShadow()
     */
    void bindShadow(bool enable) const;

    //! Bind the current texture.
    void bindTexture(bool bindOnlyFirstTexture = false) const;

    //! Set the shader program to be used.
    void setShaderProgram(MCGLShaderProgram * program);

    //! Set the shader program to be used for 2d shadows.
    void setShadowShaderProgram(MCGLShaderProgram * program);

    //! Get the shader program to be used.
    MCGLShaderProgram * shaderProgram() const;

    //! Get the shader program to be used for 2d shadows.
    MCGLShaderProgram * shadowShaderProgram() const;

    //! Get width
    MCFloat width() const;

    //! Get height
    MCFloat height() const;

private:

    void init(const FaceVector & faces);

    void initVBOs(
        const MCGLVertex   * vertices,
        const MCGLVertex   * normals,
        const MCGLTexCoord * texCoords,
        const GLfloat      * colors);

    void doRender(bool autoBind);
    void doRenderShadow(bool autoBind);

    int                 m_numVertices;
    GLuint              m_handle1;
    GLuint              m_handle2;
    MCFloat             m_w, m_h;
    GLuint              m_vbo;
    GLuint              m_vao;
    GLfloat             m_r, m_g, m_b, m_a;
    MCFloat             m_sx, m_sy, m_sz;
    MCGLShaderProgram * m_program;
    MCGLShaderProgram * m_shadowProgram;
};

#endif // MCMESH_HH
