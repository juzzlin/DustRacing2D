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

#include "renderer.hpp"

#include "eventhandler.hpp"
#include "fontfactory.hpp"
#include "game.hpp"
#include "scene.hpp"

#ifdef __MC_GL30__
#include "shaders30.h"
#else
#include "shaders.h" // These are also for GLES now
#endif

#include "../common/config.hpp"

#include <MCAssetManager>
#include <MCGLScene>
#include <MCSurface>
#include <MCSurfaceManager>
#include <MCTrigonom>

#include "simple_logger.hpp"

#include <cassert>
#include <cmath>

#include <QDir>
#include <QFontDatabase>
#include <QIcon>
#include <QKeyEvent>
#include <QOpenGLFramebufferObject>
#include <QScreen>

Renderer * Renderer::m_instance = nullptr;

Renderer::Renderer(int hRes, int vRes, int fullHRes, int fullVRes, bool fullScreen, MCGLScene & glScene)
  : m_hRes { hRes }
  , m_vRes { vRes }
  , m_fullHRes { fullHRes }
  , m_fullVRes { fullVRes }
  , m_fullScreen { fullScreen }
  , m_glScene { glScene }
{
    assert(!Renderer::m_instance);
    Renderer::m_instance = this;

    juzzlin::L().info() << "Pixel scale: " << static_cast<int>(devicePixelRatio() * 100) << "%";

    setSurfaceType(QWindow::OpenGLSurface);

    setTitle(QString(Config::Game::GAME_NAME) + " " + Config::Game::GAME_VERSION);
    setIcon(QIcon(":/dustrac-game.png"));
}

Renderer & Renderer::instance()
{
    assert(Renderer::m_instance);
    return *Renderer::m_instance;
}

void Renderer::resizeWindow()
{
    // Set window size & disable resize
    const int windowHRes = static_cast<int>(m_hRes / devicePixelRatio());
    const int windowVRes = static_cast<int>(m_vRes / devicePixelRatio());
    resize(windowHRes, windowVRes);
    setMinimumSize({ windowHRes, windowVRes });
    setMaximumSize({ windowHRes, windowVRes });
}

void Renderer::centerWindow()
{
    // Try to center the window
    const int windowHRes = static_cast<int>(m_hRes / devicePixelRatio());
    const int windowVRes = static_cast<int>(m_vRes / devicePixelRatio());
    const int screenHRes = static_cast<int>(m_fullHRes / devicePixelRatio());
    const int screenVRes = static_cast<int>(m_fullVRes / devicePixelRatio());
    setPosition(screenHRes / 2 - windowHRes / 2, screenVRes / 2 - windowVRes / 2);
}

void Renderer::initialize()
{
    juzzlin::L().info() << "OpenGL Version: " << glGetString(GL_VERSION);

    if (!m_fullScreen)
    {
        resizeWindow();
        centerWindow();
    }

    m_glScene.initialize();

    loadShaders();
    loadFonts();

    m_rendererInitialized = true;
    emit initialized();
}

void Renderer::resizeGlScene(int viewWidth, int viewHeight)
{
    m_glScene.resize(viewWidth, viewHeight, Scene::width(), Scene::height(), m_viewAngle, m_zNear, m_zFar);
}

void Renderer::createProgramFromSource(std::string handle, std::string vshSource, std::string fshSource)
{
    // Inject precision qualifiers
#ifdef __MC_GLES__
    QString origVsh(vshSource.c_str());
    origVsh.replace("#version 120", "#version 100\nprecision mediump float;\nprecision mediump int;\n");
    vshSource = origVsh.toStdString();

    QString origFsh(fshSource.c_str());
    origFsh.replace("#version 120", "#version 100\nprecision mediump float;\nprecision mediump int;\n");
    fshSource = origFsh.toStdString();
#endif

    // Note: ShaderProgram throws on error.
    m_shaderHash[handle] = std::make_shared<MCGLShaderProgram>(vshSource, fshSource);
}

void Renderer::loadShaders()
{
    // Engine defaults
    m_shaderHash["default"] = MCGLScene::instance().defaultShaderProgram();
    m_shaderHash["defaultSpecular"] = MCGLScene::instance().defaultSpecularShaderProgram();
    m_shaderHash["defaultShadow"] = MCGLScene::instance().defaultShadowShaderProgram();
    m_shaderHash["text"] = MCGLScene::instance().defaultTextShaderProgram();
    m_shaderHash["textShadow"] = MCGLScene::instance().defaultTextShadowShaderProgram();

    // Custom shaders
    createProgramFromSource("car", carVsh, carFsh);
    createProgramFromSource("fbo", fboVsh, fboFsh);
    createProgramFromSource("menu", menuVsh, menuFsh);
    createProgramFromSource("tile2d", tileVsh, MCGLShaderProgram::getDefaultFragmentShaderSource());
    createProgramFromSource("tile3d", tileVsh, tile3dFsh);
}

void Renderer::loadFonts()
{
#ifndef SYSTEM_FONTS
    QStringList fonts = { "DejaVuSans-Bold.ttf" };
    for (auto && font : fonts)
    {
        const auto path = QString(Config::General::dataPath) + QDir::separator() + "fonts" + QDir::separator() + font;
        juzzlin::L().info() << "Loading font " << path.toStdString() << "..";
        QFile fontFile(path);
        fontFile.open(QFile::ReadOnly);
        const int appFontId = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
        if (appFontId < 0)
        {
            juzzlin::L().warning() << "Failed to load font " << path.toStdString() << "..";
        }
        else
        {
            juzzlin::L().info() << "Loaded font " << QFontDatabase::applicationFontFamilies(appFontId).at(0).toStdString();
        }
    }
#endif

    MCAssetManager::instance().textureFontManager().createFontFromData(FontFactory::generateFontData("DejaVu Sans"));
}

void Renderer::setEnabled(bool enable)
{
    m_enabled = enable;
}

MCGLShaderProgramPtr Renderer::program(const std::string & id)
{
    if (const auto program { m_shaderHash[id] }; program)
    {
        return program;
    }
    else
    {
        throw std::runtime_error("Cannot find shader program '" + id + "'");
    }
}

void Renderer::setFadeValue(float value)
{
    m_fadeValue = value;
}

void Renderer::setResolution(QSize resolution)
{
    m_hRes = resolution.width();
    m_vRes = resolution.height();

    m_fbo.reset();

    m_shadowFbo.reset();
}

float Renderer::fadeValue() const
{
    return m_fadeValue;
}

QSize Renderer::resolution() const
{
    return { m_hRes, m_vRes };
}

bool Renderer::fullScreen() const
{
    return m_fullScreen;
}

void Renderer::initializeFrameBufferObjects()
{
    if (!m_fbo)
    {
        m_fbo = std::make_unique<QOpenGLFramebufferObject>(m_hRes, m_vRes);
        m_fbo->setAttachment(QOpenGLFramebufferObject::Depth);
    }

    if (!m_shadowFbo)
    {
        m_shadowFbo = std::make_unique<QOpenGLFramebufferObject>(m_hRes, m_vRes);
        m_shadowFbo->setAttachment(QOpenGLFramebufferObject::Depth);
    }
}

void Renderer::initializeMaterial()
{
    if (!m_material)
    {
        m_material = std::make_shared<MCGLMaterial>();
    }
}

void Renderer::renderObjects()
{
    m_fbo->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_scene->renderTrack();
    m_scene->renderMenu();
    m_scene->renderWorld(MCRenderGroup::Objects, true);
}

void Renderer::renderHud()
{
#ifdef DISABLE_FRAMEBUFFER_BLITS
    m_scene->renderWorld(MCRenderGroup::ObjectShadows);
    m_scene->renderWorld(MCRenderGroup::Particles);
    m_scene->renderWorld(MCRenderGroup::ParticleShadows);
    m_scene->renderHUD();
    m_scene->renderCommonHUD();
    m_fbo->release();
#else
    m_fbo->release();
    m_shadowFbo->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    QOpenGLFramebufferObject::blitFramebuffer(m_shadowFbo.get(), m_fbo.get(), GL_DEPTH_BUFFER_BIT);
    m_scene->renderWorld(MCRenderGroup::ObjectShadows);
    m_scene->renderWorld(MCRenderGroup::ParticleShadows);
    m_shadowFbo->release();

    m_fbo->bind();
    m_material->setTexture(m_shadowFbo->texture(), 0);
    m_material->setAlphaBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    MCSurface dummySurface { "dummy", m_material, 2.0f, 2.0f };
    dummySurface.setColor(MCGLColor { 1, 1, 1, 0.5f });
    dummySurface.setShaderProgram(program("fbo"));
    dummySurface.bind();
    dummySurface.render(nullptr, {}, 0);
    m_scene->renderWorld(MCRenderGroup::Particles); // Render particles here to avoid glitches due to transparency
    m_scene->renderHUD();
    m_scene->renderCommonHUD();
    m_fbo->release();
#endif
}

void Renderer::renderScreen()
{
    if (m_fullScreen)
    {
        resizeGlScene(static_cast<int>(m_fullHRes * devicePixelRatio()),
                      static_cast<int>(m_fullVRes * devicePixelRatio()));
    }
    else
    {
        resizeGlScene(static_cast<int>(m_hRes * devicePixelRatio()),
                      static_cast<int>(m_vRes * devicePixelRatio()));
    }

    m_material->setTexture(m_fbo->texture(), 0);
    m_material->setAlphaBlend(false);
    MCSurface dummySurface { "dummy", m_material, 2.0f, 2.0f };
    dummySurface.setShaderProgram(program("fbo"));
    dummySurface.bind();
    dummySurface.render(nullptr, {}, 0);
}

void Renderer::render()
{
    if (!m_scene)
    {
        return;
    }

    resizeGlScene(m_hRes, m_vRes);

    initializeFrameBufferObjects();
    initializeMaterial();

    renderObjects();
    renderHud();
    renderScreen();
}

void Renderer::renderLater()
{
    if (!m_updatePending)
    {
        m_updatePending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool Renderer::event(QEvent * event)
{
    switch (event->type())
    {
    case QEvent::UpdateRequest:
        m_updatePending = false;
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void Renderer::exposeEvent(QExposeEvent *)
{
    if (isExposed())
    {
        renderNow();
    }
}

void Renderer::renderNow()
{
    if (!isExposed())
    {
        return;
    }

    bool needsInitialize = false;

    if (!m_context)
    {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        if (!m_context->isValid())
        {
            std::stringstream ss;
            ss << "Cannot create context for OpenGL version " << requestedFormat().majorVersion() << "." << requestedFormat().minorVersion();
            throw std::runtime_error(ss.str());
        }

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize)
    {
        initializeOpenGLFunctions();
        initialize();
    }

    m_frameCounter++;
    if (Game::instance().fps() == Game::Fps::Fps60 || (Game::instance().fps() == Game::Fps::Fps30 && m_frameCounter & 0x01))
    {
        render();

        m_context->swapBuffers(this);
    }
}

void Renderer::resizeEvent(QResizeEvent * event)
{
    if (m_rendererInitialized)
        resizeGlScene(event->size().width(), event->size().height());
}

void Renderer::keyPressEvent(QKeyEvent * event)
{
    assert(m_eventHandler);
    if (!m_eventHandler->handleKeyPressEvent(event))
    {
        QWindow::keyPressEvent(event);
    }
}

void Renderer::keyReleaseEvent(QKeyEvent * event)
{
    assert(m_eventHandler);
    if (!m_eventHandler->handleKeyReleaseEvent(event))
    {
        QWindow::keyReleaseEvent(event);
    }
}

void Renderer::mousePressEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMousePressEvent(event, width(), height(), true);
}

void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMouseReleaseEvent(event, width(), height(), true);
}

void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMouseMoveEvent(event);
}

void Renderer::setScene(Scene & scene)
{
    m_scene = &scene;
}

void Renderer::setEventHandler(EventHandler & eventHandler)
{
    m_eventHandler = &eventHandler;
}

Renderer::~Renderer()
{
    // Ensure that OpenGL stuff gets deleted before the OpenGL context deletion

    m_fbo.reset();
    m_shadowFbo.reset();
    m_shaderHash.clear();

    delete m_context;
}
