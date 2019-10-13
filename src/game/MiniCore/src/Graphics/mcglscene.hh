// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCGLSCENE_HH
#define MCGLSCENE_HH

#include "mcglshaderprogram.hh"
#include <MCGLM>

#include <vector>

class MCGLAmbientLight;
class MCGLDiffuseLight;

/*! Basic initializations of an OpenGL scene suitable for
 *  2D/2.5D games. Internally MCGLScene uses glm and glew libraries. */
class MCGLScene
{
public:
    //! Viewport split type for two-player setup.
    enum SplitType
    {
        ShowFullScreen = 0,
        ShowOnLeft,
        ShowOnRight,
        ShowOnTop,
        ShowOnBottom
    };

    //! Constructor.
    MCGLScene();

    //! Destructor.
    virtual ~MCGLScene();

    //! \return the singleton instance.
    static MCGLScene & instance();

    //! Initializes OpenGL and GLEW. Re-implement if desired.
    virtual void initialize();

    //! Resize / set projection matrices and viewport. Re-implement if desired.
    virtual void resize(unsigned int viewWidth, unsigned int viewHeight,
                        unsigned int sceneWidth, unsigned int sceneHeight, float viewAngle, float zNear, float zFar);

    //! Set viewport split type.
    void setSplitType(SplitType splitType = ShowFullScreen);

    //! \return the resulting view projection matrix to be used in shaders.
    const glm::mat4 & viewProjectionMatrix() const;

    /*! Set ambient light for all registered shaders. */
    void setAmbientLight(const MCGLAmbientLight & light);

    /*! Set diffuse light for all registered shaders. */
    void setDiffuseLight(const MCGLDiffuseLight & light);

    /*! Set specular light for all registered shaders. */
    void setSpecularLight(const MCGLDiffuseLight & light);

    //! Set fade value (0.0..1.0) to fade out/in the scene.
    void setFadeValue(float value);

    //! \return default shader program.
    MCGLShaderProgramPtr defaultShaderProgram();

    //! \return default specular shader program.
    MCGLShaderProgramPtr defaultSpecularShaderProgram();

    //! \return default shadow shader program.
    MCGLShaderProgramPtr defaultShadowShaderProgram();

    //! \return default shader program for text.
    MCGLShaderProgramPtr defaultTextShaderProgram();

    //! \return default shader program for text shadow.
    MCGLShaderProgramPtr defaultTextShadowShaderProgram();

    //! \return default shader program for FBO use.
    MCGLShaderProgramPtr defaultFBOShaderProgram();

    //! \return current view angle
    float viewAngle() const;

    //! \return current eye distance
    float eyeZ() const;

protected:
    //! Set viewer's position. Automatically called by resize().
    virtual void setViewerPosition(unsigned int sceneWidth, unsigned int sceneHeight, float viewAngle);

    //! Set projection. Automatically called by resize().
    virtual void setProjection(float aspectRatio, float zNear, float zFar, float viewAngle);

private:
    /*! Adds a shader program. Added programs are updated when e.g.
     *  the projection changes. MCGLShaderProgram calls this when linked. */
    void addShaderProgram(MCGLShaderProgram & shader);

    void createDefaultShaderPrograms();

    void updateViewport();

    void updateViewProjectionMatrixAndShaders();

    SplitType m_splitType;

    unsigned int m_viewWidth;

    unsigned int m_viewHeight;

    unsigned int m_sceneWidth;

    unsigned int m_sceneHeight;

    float m_viewAngle;

    float m_eyeZ;

    float m_zNear;

    float m_zFar;

    float m_fadeValue;

    glm::mat4 m_viewMatrix;

    glm::mat4 m_projectionMatrix;

    mutable glm::mat4 m_viewProjectionMatrix;

    mutable bool m_updateViewProjection;

    std::vector<MCGLShaderProgram *> m_shaders;

    MCGLShaderProgramPtr m_defaultShader;

    MCGLShaderProgramPtr m_defaultSpecularShader;

    MCGLShaderProgramPtr m_defaultShadowShader;

    MCGLShaderProgramPtr m_defaultTextShader;

    MCGLShaderProgramPtr m_defaultTextShadowShader;

    MCGLShaderProgramPtr m_defaultFBOShader;

    static MCGLScene * m_instance;

    friend class MCGLShaderProgram;
};

#endif // MCGLSCENE_HH
