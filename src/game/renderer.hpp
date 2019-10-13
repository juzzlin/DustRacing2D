// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <MCGLEW>
#include <MCGLShaderProgram>

#include "eventhandler.hpp"

#include <QOpenGLFunctions>
#include <QWindow>

#include <memory>
#include <string>
#include <unordered_map>

class InputHandler;
class QKeyEvent;
class QOpenGLFramebufferObject;
class QPaintEvent;
class Scene;
class MCGLScene;

//! The singleton renderer widget and the main "window".
class Renderer : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    //! Constructor.
    Renderer(int hRes, int vRes, bool fullScreen, MCGLScene & glScene);

    //! Destructor.
    virtual ~Renderer();

    //! \return the single instance.
    static Renderer & instance();

    void initialize();

    //! Set game scene to be rendered.
    void setScene(Scene & scene);

    //! Set event handler to be used.
    void setEventHandler(EventHandler & eventHandler);

    //! \return shader program object by the given id string.
    MCGLShaderProgramPtr program(const std::string & id);

    //! \return scene face factor 0.0..1.0.
    float fadeValue() const;

    QSize resolution() const
    {
        return QSize(m_hRes, m_vRes);
    }

    bool fullScreen() const
    {
        return m_fullScreen;
    }

signals:

    void closed();

    void initialized();

public slots:

    void setEnabled(bool enable);

    void setFadeValue(float value);

    void setResolution(QSize resolution);

    void renderLater();

    void renderNow();

protected:
    //! \reimp
    bool event(QEvent *) override;

    //! \reimp
    void exposeEvent(QExposeEvent *) override;

    //! \reimp
    void resizeEvent(QResizeEvent *) override;

    //! \reimp
    void keyPressEvent(QKeyEvent * event) override;

    //! \reimp
    void keyReleaseEvent(QKeyEvent * event) override;

    //! \reimp
    void mousePressEvent(QMouseEvent * event) override;

    //! \reimp
    void mouseReleaseEvent(QMouseEvent * event) override;

    //! \reimp
    void mouseMoveEvent(QMouseEvent * event) override;

private:
    //! Load vertex and fragment shaders.
    void loadShaders();

    void loadFonts();

    void createProgramFromSource(std::string handle, std::string vshSource, std::string fshSource);

    void render();

    void resizeGL(int viewWidth, int viewHeight);

    typedef std::unordered_map<std::string, MCGLShaderProgramPtr> ShaderHash;

    QOpenGLContext * m_context;

    Scene * m_scene;

    EventHandler * m_eventHandler;

    const float m_viewAngle;

    const float m_zNear;

    const float m_zFar;

    float m_fadeValue;

    ShaderHash m_shaderHash;

    bool m_enabled;

    int m_hRes;

    int m_vRes;

    int m_fullHRes;

    int m_fullVRes;

    int m_frameCounter;

    bool m_fullScreen;

    bool m_updatePending;

    static Renderer * m_instance;

    std::unique_ptr<QOpenGLFramebufferObject> m_fbo;

    std::unique_ptr<QOpenGLFramebufferObject> m_shadowFbo;

    MCGLScene & m_glScene;
};

#endif // RENDERER_HPP
