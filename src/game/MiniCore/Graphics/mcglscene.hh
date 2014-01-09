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
#include "mctypes.hh"
#include <vector>

class MCGLShaderProgram;
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

    /*! Adds a shader program. Added programs are updated when e.g.
     *  the projection changes. */
    void addShaderProgram(MCGLShaderProgram & shader);

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

protected:

    //! Set viewer's position. Automatically called by resize().
    virtual void setViewerPosition(MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle);

    //! Set projection. Automatically called by resize().
    virtual void setProjection(MCFloat aspectRatio, MCFloat zNear, MCFloat zFar, MCFloat viewAngle);

private:

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
};

#endif // MCGLSCENE_HH
