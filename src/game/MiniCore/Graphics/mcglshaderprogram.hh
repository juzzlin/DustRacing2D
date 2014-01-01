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

#include "mcglambientlight.hh"
#include "mcglcolor.hh"
#include "mcgldiffuselight.hh"
#include "mctypes.hh"
#include "mcvector3d.hh"

#include <string>

class MCGLScene;

/*! Base class for GLSL shader programs compatible with MiniCore.
 *  The user needs to inherit from this class and re-implement the
 *  desired features so that they are forwarded to the actual
 *  shaders as uniforms. */
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
    virtual bool addVertexShaderFromSource(const std::string & source);

    /*! Add a fragment shader.
     *  \return true if succeeded. */
    virtual bool addFragmentShaderFromSource(const std::string & source);

    /*! Get the default vertex shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultVertexShaderSource();

    /*! Get the default fragment shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultFragmentShaderSource();

    /*! Get the default fragment shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultSpecularFragmentShaderSource();

    /*! Get the default shadow vertex shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultShadowVertexShaderSource();

    /*! Get the default shadow fragment shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultShadowFragmentShaderSource();

    /*! Add a geometry shader.
     *  \return true if succeeded. */
    virtual bool addGeometryShaderFromSource(const std::string & source);

    //! Set view projection matrix. Done automatically by MCGLScene.
    virtual void setViewProjectionMatrix(const glm::mat4x4 & viewProjectionMatrix);

    //! Set transformation: rotation (about Z-axis) + position.
    virtual void setTransform(GLfloat angle, const MCVector3dF & pos);

    //! Set color.
    virtual void setColor(const MCGLColor & color);

    //! Set scale.
    virtual void setScale(GLfloat x = 1.0, GLfloat y = 1.0, GLfloat z = 1.0);

    /*! Set fade factor. The final color should be multiplied with this factor. */
    virtual void setFadeValue(GLfloat f = 1.0);

    //! Bind given texture unit to the sampler 0.
    virtual void bindTextureUnit0(GLuint index = 0);

    //! Bind given texture unit to the sampler 1 for multitexturing.
    virtual void bindTextureUnit1(GLuint index = 1);

    //! Bind given texture unit to the sampler 2 for multitexturing.
    virtual void bindTextureUnit2(GLuint index = 2);

    //! Set ambient light.
    virtual void setAmbientLight(const MCGLAmbientLight & light);

    //! Set diffuse light.
    virtual void setDiffuseLight(const MCGLDiffuseLight & light);

    //! Set point size for point sprites.
    virtual void setPointSize(GLfloat pointSize);

private:

    static MCGLShaderProgram * m_activeProgram;

    MCGLScene & m_scene;
    bool        m_isBound;
    GLuint      m_program;
    GLuint      m_fragmentShader;
    GLuint      m_vertexShader;
};

#endif // MCGLSHADERPROGRAM_HH
