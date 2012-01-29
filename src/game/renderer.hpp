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

#include <QGLWidget>
#include <QTimer>

class InputHandler;
class MCCamera;
class QKeyEvent;
class QPaintEvent;
class Scene;

//! The game renderer widget.
class Renderer : public QGLWidget
{
    Q_OBJECT

public:

    //! Constructor.
    Renderer(QWidget * parent = nullptr);

    //! Destructor.
    virtual ~Renderer();

    void updateFrame(MCCamera * pCamera);

    //! Set game scene to be rendered.
    void setScene(Scene * pScene);

    //! Set input handler to be used.
    void setInputHandler(InputHandler * pInputHandler);

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

    //! Set viewer position so that the scene looks like
    //! a pure 2D-scene.
    void setViewerPosition(int viewWidth, int viewHeight) const;

    Scene        * m_pScene;
    MCCamera     * m_pCamera;
    InputHandler * m_pInputHandler;
    const float    m_viewAngle;
};

#endif // RENDERER_HPP
