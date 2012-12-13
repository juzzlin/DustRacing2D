// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCGLSHADERPROGRAM_HH
#define MCGLSHADERPROGRAM_HH

#include <MCGLEW>
#include <MCGLM>

#include "mctypes.hh"
#include "mcvector3d.hh"

#include <string>

class MCGLScene;

/*! Base class for GLSL shader programs compatible with MiniCore.
 *  The user needs to inherit from this class and re-implement the
 *  desired features so that they are forwarded to the actual
 *  shaders as uniforms. The default implementation does nothing. */
class MCGLShaderProgram
{
public:

    //! Assumed vertex attribute locations.
    enum VertexAttribLocations
    {
        VAL_Vertex    = 0,
        VAL_Normal    = 1,
        VAL_TexCoords = 2,
        VAL_Color     = 3
    };

    //! Constructor.
    MCGLShaderProgram(MCGLScene & scene);

    //! Destructor.
    virtual ~MCGLShaderProgram();

    /*! Binds the program. Re-imp and call parent implementation.
     *  The previous active shader program will be lazily released,
     *  so usually it won't be necessary to call release(). */
    virtual void bind();

    //! Releases the program. Re-imp and call parent implementation.
    virtual void release();

    //! \return true if bound.
    bool isBound() const;

    //! Link the program.
    virtual void link();

    //! \return true if linked.
    virtual bool isLinked() const;

    /*! Add a vertex shader.
     *  \return true if succeeded. */
    virtual bool addVertexShader(const std::string & path);

    /*! Add a fragment shader.
     *  \return true if succeeded. */
    virtual bool addFragmentShader(const std::string & path);

    //! Set model view projection matrix.
    virtual void setModelViewProjectionMatrix(
        const glm::mat4x4 & modelViewProjectionMatrix);

    //! Set rotation (about Z-axis).
    virtual void rotate(GLfloat angle);

    //! Set translation.
    virtual void translate(const MCVector3dF & pos);

    //! Set color.
    virtual void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0);

    //! Set scale.
    virtual void setScale(GLfloat x = 1.0, GLfloat y = 1.0, GLfloat z = 1.0);

    /*! Set fade factor. The final color should be multiplied with this
     *  factor. */
    virtual void setFadeValue(GLfloat f = 1.0);

    //! Bind given texture unit to the sampler 0.
    virtual void bindTextureUnit0(GLuint index);

    //! Bind given texture unit to the sampler 1 for multitexturing.
    virtual void bindTextureUnit1(GLuint index);

private:

    static MCGLShaderProgram * m_activeProgram;

    MCGLScene & m_scene;
    bool        m_isBound;
};

#endif // MCGLSHADERPROGRAM_HH
