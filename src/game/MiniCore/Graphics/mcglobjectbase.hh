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

class MCGLShaderProgram;

/*! Base class for GL objects in MiniCore. Automatically creates VBO, VAO and
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
    void bind();

    //! Helper to bind texturing and VAO for shadow rendering.
    void bindShadow();

    //! Bind the current matrial setup.
    void bindMaterial(bool bindOnlyFirstTexture = false, bool bindForShadow = false);

    //! Set the shader program to be used.
    void setShaderProgram(MCGLShaderProgram * program);

    //! Set the shader program to be used for 2d shadows.
    void setShadowShaderProgram(MCGLShaderProgram * program);

    //! Get the shader program to be used.
    MCGLShaderProgram * shaderProgram() const;

    //! Get the shader program to be used for 2d shadows.
    MCGLShaderProgram * shadowShaderProgram() const;

    //! Set material.
    void setMaterial(MCGLMaterialPtr material);

    //! Get material if set.
    MCGLMaterialPtr material() const;

private:

    GLuint              m_vao;
    GLuint              m_vbo;
    MCGLShaderProgram * m_program;
    MCGLShaderProgram * m_shadowProgram;
    MCGLMaterialPtr     m_material;
};

#endif // MCGLOBJECTBASE_HH
