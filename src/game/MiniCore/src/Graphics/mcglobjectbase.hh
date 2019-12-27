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

#ifndef MCGLOBJECTBASE_HH
#define MCGLOBJECTBASE_HH

#include "mcglmaterial.hh"
#include "mcglshaderprogram.hh"
#include "mcgltexcoord.hh"
#include "mcglvertex.hh"
#include <MCGLEW>

class MCCamera;

/*! Base class for GL renderables in MiniCore. Automatically creates VBO, VAO and
 *  basic texturing support. */
#ifdef __MC_QOPENGLFUNCTIONS__
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
class MCGLObjectBase : protected QOpenGLFunctions
#else
class MCGLObjectBase
#endif
{
public:
    //! Constructor.
    explicit MCGLObjectBase(std::string handle);

    //! Destructor.
    virtual ~MCGLObjectBase();

    //! Create the VAO. Return false if VAO not available.
    bool createVAO();

    //! Bind the VAO. VAO will be created when constructed.
    void bindVAO();

    //! Release the VAO.
    void releaseVAO();

    //! Create the VBO.
    void createVBO();

    //! Bind the VBO. VBO will be created when constructed.
    void bindVBO();

    //! Release the VBO.
    void releaseVBO();

    /*! Render by using the default size.
     * \param pos The position.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height. */
    virtual void render(MCCamera * camera, MCVector3dFR pos, float angle);

    //! Render (fake) shadow
    virtual void renderShadow(MCCamera * camera, MCVector3dFR pos, float angle);

    //! Render the vertex buffer only. bind() must be called separately.
    virtual void render();

    //! Helper to bind texturing and VAO.
    virtual void bind();

    //! Helper to bind texturing and VAO for shadow rendering.
    virtual void bindShadow();

    //! Helper to bind texturing and VAO.
    virtual void release();

    //! Helper to bind texturing and VAO for shadow rendering.
    virtual void releaseShadow();

    //! Set the shader program to be used.
    void setShaderProgram(MCGLShaderProgramPtr program);

    //! Set the shader program to be used for 2d shadows.
    void setShadowShaderProgram(MCGLShaderProgramPtr program);

    //! Get the shader program to be used.
    MCGLShaderProgramPtr shaderProgram() const;

    //! Get the shader program to be used for 2d shadows.
    MCGLShaderProgramPtr shadowShaderProgram() const;

    //! Set material.
    void setMaterial(MCGLMaterialPtr material);

    //! Get material if set.
    MCGLMaterialPtr material() const;

    //! Get vertex
    const MCGLVertex & vertex(size_t index) const;

    //! Get vertices
    const GLfloat * verticesAsGlArray() const;

    size_t vertexCount() const;

    const MCGLVertex & normal(size_t index) const;

    const GLfloat * normalsAsGlArray() const;

    //! Set mesh color.
    void setColor(const MCGLColor & color);

    //! Get mesh color.
    MCGLColor color() const;

    //! Get a vertex color, needed for batching
    const MCGLColor & color(size_t index) const;

    //! Get a vertex colors
    const GLfloat * colorsAsGlArray() const;

    //! Get texture coord of a vertex
    const MCGLTexCoord & texCoord(size_t index) const;

    //! Get texture coords
    const GLfloat * texCoordsAsGlArray() const;

    //! Get scale
    MCVector3dF scale() const;

    //! Set scaling factors.
    void setScale(const MCVector3dF & scale);

    //! Set object size. Actually this just calculates the corresponding scale.
    void setSize(float width, float height);

    //! Get width
    float width() const;

    void setWidth(float width);

    //! Get height
    float height() const;

    void setHeight(float height);

    //! Get minimum Z
    float minZ() const;

    void setMinZ(float minZ);

    //! Get maximum Z
    float maxZ() const;

    void setMaxZ(float maxZ);

    std::string handle() const;

    void setHandle(const std::string & handle);

protected:
    //! Store a vertex, needed for batching
    void addVertex(const MCGLVertex & vertex);

    //! Set vertices
    template<typename T>
    using Container = std::vector<T>;
    using VertexVector = Container<MCGLVertex>;
    void setVertices(const VertexVector & vertices);

    //! Store a normal, needed for batching
    void addNormal(const MCGLVertex & normal);

    //! Set normals
    void setNormals(const VertexVector & normals);

    //! Store a tex coord, needed for batching
    void addTexCoord(const MCGLTexCoord & texCoord);

    //! Set texture coords
    using TexCoordVector = Container<MCGLTexCoord>;
    void setTexCoords(const TexCoordVector & texCoords);

    //! Store a vertex color, needed for batching
    void addColor(const MCGLColor & color);

    //! Set vertex colors
    using ColorVector = Container<MCGLColor>;
    void setColors(const ColorVector & colors);

    //! This should be called after setting vertices, texture coords, normals and colors
    void initBufferData(size_t totalDataSize, GLuint drawType = GL_STATIC_DRAW);

    void addBufferSubData(MCGLShaderProgram::VertexAttribLocations dataType, size_t dataSize, const GLfloat * data);

    void addBufferSubData(MCGLShaderProgram::VertexAttribLocations dataType, size_t dataSize, size_t offsetJump, const GLfloat * data);

    void finishBufferData();

    void initUpdateBufferData();

    size_t totalDataSize() const;

    virtual void setAttributePointers();

    void enableAttributePointers();

    void disableAttributePointers();

private:
    static GLuint m_boundVbo;

    std::string m_handle;

#ifdef __MC_QOPENGLFUNCTIONS__
    QOpenGLVertexArrayObject m_vao;
#else
    GLuint m_vao = 0;
#endif
    GLuint m_vbo = 0;

    MCGLShaderProgramPtr m_program;

    MCGLShaderProgramPtr m_shadowProgram;

    MCGLMaterialPtr m_material;

    size_t m_bufferDataOffset = 0;

    size_t m_vertexDataSize = 0;

    size_t m_normalDataSize = 0;

    size_t m_texCoordDataSize = 0;

    size_t m_colorDataSize = 0;

    size_t m_totalDataSize = 0;

    bool m_hasVao = false;

    VertexVector m_vertices;

    VertexVector m_normals;

    std::vector<MCGLTexCoord> m_texCoords;

    std::vector<MCGLColor> m_colors;

    MCGLColor m_color;

    float m_width = 0;

    float m_height = 0;

    float m_minZ = 0;

    float m_maxZ = 0;

    MCVector3dF m_scale = MCVector3dF(1.0f, 1.0f, 1.0f);

    friend class MCGLRectParticle; // Direct access to protected methods without inheritance
};

#endif // MCGLOBJECTBASE_HH
