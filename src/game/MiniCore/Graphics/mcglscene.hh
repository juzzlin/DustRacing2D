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

#ifndef MCGLSCENE_HH
#define MCGLSCENE_HH

#include <MCGLM>
#include "mcglshaderprogram.hh"
#include "mctypes.hh"
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

    //! Get the singleton instance.
    static MCGLScene & instance();

    //! Initializes OpenGL and GLEW. Re-implement if desired.
    virtual void initialize();

    //! Resize / set projection matrices and viewport. Re-implement if desired.
    virtual void resize(MCUint viewWidth, MCUint viewHeight,
        MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle);

    //! Set viewport split type.
    void setSplitType(SplitType splitType = ShowFullScreen);

    //! Get the resulting view projection matrix to be used in shaders.
    const glm::mat4 & viewProjectionMatrix() const;

    /*! Set fade value used in fade in/out animations.
     *  This sets the fade uniform in all registered shaders. */
    void setFadeValue(MCFloat value);

    /*! Set ambient light for all registered shaders. */
    void setAmbientLight(const MCGLAmbientLight & light);

    /*! Set diffuse light for all registered shaders. */
    void setDiffuseLight(const MCGLDiffuseLight & light);

    /*! Set specular light for all registered shaders. */
    void setSpecularLight(const MCGLDiffuseLight & light);

    //! Get default shader program.
    MCGLShaderProgramPtr defaultShaderProgram();

    //! Get default specular shader program.
    MCGLShaderProgramPtr defaultSpecularShaderProgram();

    //! Get default shadow shader program.
    MCGLShaderProgramPtr defaultShadowShaderProgram();

protected:

    //! Set viewer's position. Automatically called by resize().
    virtual void setViewerPosition(MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle);

    //! Set projection. Automatically called by resize().
    virtual void setProjection(MCFloat aspectRatio, MCFloat zNear, MCFloat zFar, MCFloat viewAngle);

private:

    /*! Adds a shader program. Added programs are updated when e.g.
     *  the projection changes. MCGLShaderProgram calls this when linked. */
    void addShaderProgram(MCGLShaderProgram & shader);

    void createDefaultShaderPrograms();

    void updateViewport();

    void updateViewProjectionMatrixAndShaders();

    SplitType                        m_splitType;
    MCUint                           m_viewWidth;
    MCUint                           m_viewHeight;
    MCUint                           m_sceneWidth;
    MCUint                           m_sceneHeight;
    MCFloat                          m_viewAngle;
    glm::mat4                        m_viewMatrix;
    glm::mat4                        m_projectionMatrix;
    mutable glm::mat4                m_viewProjectionMatrix;
    mutable bool                     m_updateViewProjection;
    std::vector<MCGLShaderProgram *> m_shaders;
    MCGLShaderProgramPtr             m_defaultShader;
    MCGLShaderProgramPtr             m_defaultSpecularShader;
    MCGLShaderProgramPtr             m_defaultShadowShader;

    static MCGLScene               * m_instance;

    friend class MCGLShaderProgram;
};

#endif // MCGLSCENE_HH
