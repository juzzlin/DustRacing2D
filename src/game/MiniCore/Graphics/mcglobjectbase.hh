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

#include <MCGLEW>
#include "mcglmaterial.hh"
#include "mcglrectparticle.hh"
#include "mcglshaderprogram.hh"

/*! Base class for GL renderables in MiniCore. Automatically creates VBO, VAO and
 *  basic texturing support. */
class MCGLObjectBase
{
public:

    //! Constructor.
    MCGLObjectBase();

    //! Destructor.
    virtual ~MCGLObjectBase();

    //! Create the VAO.
    void createVAO();

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

    //! Helper to bind texturing and VAO.
    virtual void bind();

    //! Helper to bind texturing and VAO for shadow rendering.
    virtual void bindShadow();

    //! Helper to bind texturing and VAO.
    virtual void release();

    //! Helper to bind texturing and VAO for shadow rendering.
    virtual void releaseShadow();

    //! Bind the current matrial setup.
    void bindMaterial(bool bindForShadow = false);

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

protected:

    void initBufferData(int totalDataSize, GLuint drawType = GL_STATIC_DRAW);

    void addBufferSubData(
        MCGLShaderProgram::VertexAttribLocations dataType, int dataSize, const GLfloat * data);

    void addBufferSubData(
        MCGLShaderProgram::VertexAttribLocations dataType, int dataSize, int offsetJump, const GLfloat * data);

    void finishBufferData();

    void initUpdateBufferData();

private:

    void setAttributePointers();

    static GLuint m_boundVbo;

    GLuint m_vao;

    GLuint m_vbo;

    MCGLShaderProgramPtr m_program;

    MCGLShaderProgramPtr m_shadowProgram;

    MCGLMaterialPtr m_material;

    int m_bufferDataOffset;

    int m_vertexDataSize;

    int m_normalDataSize;

    int m_texCoordDataSize;

    int m_colorDataSize;

    friend class MCGLRectParticle; // Direct access to protected methods without inheritance
};

#endif // MCGLOBJECTBASE_HH
