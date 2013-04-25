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

#include "eventhandler.hpp"

#include <QGLWidget>

#include <memory>
#include <string>
#include <unordered_map>

class InputHandler;
class MCGLScene;
class MCGLShaderProgram;
class QGLShaderProgram;
class QGLShader;
class QKeyEvent;
class QPaintEvent;
class Scene;

//! The singleton renderer widget and the main "window".
class Renderer : public QGLWidget
{
    Q_OBJECT

public:

    //! Constructor.
    Renderer(
        const QGLFormat & qglFormat,
        int hRes,
        int vRes,
        bool nativeResolution,
        bool fullScreen,
        QWidget * parent = nullptr);

    int hRes() const {return m_hRes;}
    int vRes() const {return m_vRes;}

    //! Destructor.
    virtual ~Renderer();

    static Renderer & instance();

    void updateFrame();

    //! Set game scene to be rendered.
    void setScene(Scene & scene);

    //! Set event handler to be used.
    void setEventHandler(EventHandler & eventHandler);

    MCGLShaderProgram & program(const std::string & id);

    MCGLScene & glScene();

    float fadeValue() const;

signals:

    void closed();

public slots:

    void setEnabled(bool enable);

    void setFadeValue(float value);

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

    //! \reimp
    void mousePressEvent(QMouseEvent * event);

    //! \reimp
    void mouseReleaseEvent(QMouseEvent * event);

    //! \reimp
    void closeEvent(QCloseEvent * event);

private:

    //! Load vertex and fragment shaders.
    void loadShaders();

    void createProgram(
        const std::string & handle, const std::string & fshPath, const std::string & vshPath);

    void renderNativeResolutionOrWindowed();

    void renderCustomResolution();

    typedef std::shared_ptr<MCGLShaderProgram> MCGLShaderProgramPtr;
    typedef std::unordered_map<std::string, MCGLShaderProgramPtr > ShaderHash;

    Scene           * m_scene;
    MCGLScene       * m_glScene;
    EventHandler    * m_eventHandler;
    const float       m_viewAngle;
    float             m_fadeValue;
    ShaderHash        m_shaderHash;
    bool              m_enabled;
    int               m_hRes;
    int               m_vRes;
    bool              m_nativeResolution;
    bool              m_fullScreen;
    static Renderer * m_instance;
};

#endif // RENDERER_HPP
