// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "eventhandler.hpp"

#include <QGLWidget>
#include <QTimer>

class InputHandler;
class MCCamera;
class MCGLScene;
class MCGLShaderProgram;
class QGLShaderProgram;
class QGLShader;
class QKeyEvent;
class QPaintEvent;
class Scene;

//! The singleton renderer widget.
class Renderer : public QGLWidget
{
    Q_OBJECT

public:

    //! Constructor.
    Renderer(QWidget * parent = nullptr);

    //! Destructor.
    virtual ~Renderer();

    static Renderer & instance();

    void updateFrame(MCCamera & camera);

    //! Set game scene to be rendered.
    void setScene(Scene & scene);

    //! Set event handler to be used.
    void setEventHandler(EventHandler & eventHandler);

    void setEnabled(bool enable);

    void setFadeShaderEnabled(bool enable);

    void setFadeValue(float value);

    MCGLShaderProgram & tileProgram();

    MCGLShaderProgram & masterProgram();

    MCGLShaderProgram & masterShadowProgram();

    MCGLShaderProgram & textProgram();

    MCGLShaderProgram & textShadowProgram();

    MCGLShaderProgram & particleProgram();

    MCGLShaderProgram & pointParticleProgram();

    float fadeValue() const;

protected:

    //! \reimp
    virtual void initializeGL();

    //! \reimp
    virtual void resizeGL(int viewWidth, int viewHeight);

    //! \reimp
    virtual void paintGL();

    //! \reimp
    virtual void keyPressEvent(QKeyEvent * event);

    //! \reimp
    virtual void keyReleaseEvent(QKeyEvent * event);

private:

    //! Load vertex and fragment shaders.
    void loadShaders();

    Scene             * m_pScene;
    MCGLScene         * m_pGLScene;
    MCCamera          * m_pCamera;
    EventHandler      * m_eventHandler;
    const float         m_viewAngle;
    float               m_fadeValue;
    MCGLShaderProgram * m_tileProgram;
    MCGLShaderProgram * m_masterProgram;
    MCGLShaderProgram * m_masterShadowProgram;
    MCGLShaderProgram * m_textProgram;
    MCGLShaderProgram * m_textShadowProgram;
    MCGLShaderProgram * m_particleProgram;
    MCGLShaderProgram * m_pointParticleProgram;
    bool                m_enabled;

    static Renderer * m_instance;
};

#endif // RENDERER_HPP
