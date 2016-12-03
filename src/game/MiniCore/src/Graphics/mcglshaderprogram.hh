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
#include "mcglmaterial.hh"
#include "mctypes.hh"
#include "mcvector3d.hh"

#include <map>
#include <memory>
#include <string>
#include <vector>

class MCGLScene;

/*! Base class for GLSL shader programs compatible with MiniCore.
 *  The user needs to inherit from this class and re-implement the
 *  desired features so that they are forwarded to the actual
 *  shaders as uniforms. */
#ifdef __MC_QOPENGLFUNCTIONS__
#include <QOpenGLFunctions>
class MCGLShaderProgram : protected QOpenGLFunctions
#else
class MCGLShaderProgram
#endif
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

    /*! Default constructor. MCGLScene must have been created before creating
     *  shader programs. */
    MCGLShaderProgram();

    /*! Constructor. MCGLScene must have been created before creating shader programs.
     *  Adds the given source and links the program. */
    MCGLShaderProgram(
        const std::string & vertexShaderSource, const std::string & fragmentShaderSource);

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
    virtual bool isLinked();

    /*! Add a vertex shader.
     *  \return true if succeeded. */
    virtual bool addVertexShaderFromSource(const std::string & source);

    /*! Add a fragment shader.
     *  \return true if succeeded. */
    virtual bool addFragmentShaderFromSource(const std::string & source);

    /*! Get the default vertex shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultVertexShaderSource();

    /*! Get the default vertext shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultSpecularVertexShaderSource();

    /*! Get the default fragment shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultFragmentShaderSource();

    /*! Get the default shadow vertex shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultShadowVertexShaderSource();

    /*! Get the default shadow fragment shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultShadowFragmentShaderSource();

    /*! Get the default text vertex shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultTextVertexShaderSource();

    /*! Get the default text fragment shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultTextFragmentShaderSource();

    /*! Get the default text shadow fragment shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultTextShadowFragmentShaderSource();

    /*! Get the default FBO fragment shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultFBOFragmentShaderSource();

    /*! Get the default FBO vertex shader source. Defining __MC_GLES__ will select GLES version. */
    static const char * getDefaultFBOVertexShaderSource();

    //! Push the current active program to a stack
    static void pushProgram();

    //! Pop the program stack and bind the program
    static void popProgram();

    /*! Add a geometry shader.
     *  \return true if succeeded. */
    virtual bool addGeometryShaderFromSource(const std::string & source);

    //! Set view matrix. Done automatically by MCGLScene.
    virtual void setViewMatrix(const glm::mat4x4 & viewMatrix);

    //! Set view projection matrix. Done automatically by MCGLScene.
    virtual void setViewProjectionMatrix(const glm::mat4x4 & viewProjectionMatrix);

    //! Set transformation: rotation (about Z-axis) + position.
    virtual void setTransform(GLfloat angle, const MCVector3dF & pos);

    //! Set color.
    virtual void setColor(const MCGLColor & color);

    //! Set scale.
    virtual void setScale(GLfloat x = 1.0, GLfloat y = 1.0, GLfloat z = 1.0);

    //! Bind given material.
    virtual void bindMaterial(MCGLMaterialPtr material);

    //! Set ambient light.
    virtual void setAmbientLight(const MCGLAmbientLight & light);

    //! Set diffuse light.
    virtual void setDiffuseLight(const MCGLDiffuseLight & light);

    //! Set specular light.
    virtual void setSpecularLight(const MCGLDiffuseLight & light);

    /*! Set camera location. Currently not used by MiniCore, but
     *  can be used in e.g. fragment shader effects */
    virtual void setCamera(const MCVector2dF & camera);

    virtual void setUserData1(const MCVector2dF & data);

    virtual void setUserData2(const MCVector2dF & data);

private:

    //! Uniform enums used when caching uniform locations
    enum Uniform
    {
        AmbientLightColor,
        DiffuseLightDir,
        DiffuseLightColor,
        SpecularLightDir,
        SpecularLightColor,
        SpecularCoeff,
        Tex0,
        Tex1,
        Tex2,
        ViewProjection,
        View,
        Model,
        Color,
        Scale,
        Camera,
        UserData1,
        UserData2,
    };

    void bindPendingMaterial();

    void bindTextureUnit(GLuint index, Uniform uniform);

    std::string getShaderLog(GLuint obj);

    int getUniformLocation(Uniform uniform);

    void initUniformNameMap();

    void initUniformLocationCache();

    void setPendingAmbientLight();

    void setPendingDiffuseLight();

    void setPendingSpecularLight();

    void setPendingViewProjectionMatrix();

    void setPendingViewMatrix();

    static MCGLShaderProgram * m_activeProgram;

    static std::vector<MCGLShaderProgram *> m_programStack;

    typedef std::map<Uniform, int> UniformLocationHash;
    UniformLocationHash m_uniformLocationHash;

    typedef std::map<Uniform, std::string> Uniforms;
    Uniforms m_uniforms;

    MCGLScene & m_scene;

    GLuint m_program;

    GLuint m_fragmentShader;

    GLuint m_vertexShader;

    glm::mat4x4 m_viewProjectionMatrix;

    bool m_viewProjectionMatrixPending;

    glm::mat4x4 m_viewMatrix;

    bool m_viewMatrixPending;

    MCGLDiffuseLight m_diffuseLight;

    bool m_diffuseLightPending;

    MCGLDiffuseLight m_specularLight;

    bool m_specularLightPending;

    MCGLAmbientLight m_ambientLight;

    bool m_ambientLightPending;
};

typedef std::shared_ptr<MCGLShaderProgram> MCGLShaderProgramPtr;

#endif // MCGLSHADERPROGRAM_HH
