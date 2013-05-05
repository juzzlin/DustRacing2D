// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcgame.hh"
#include "mcgameimpl.hh"
#include "mccamera.hh"
#include "mcworld.hh"
#include "mccast.hh"
#include "mcsoundmanager.hh"
#include "mcsurfacemanager.hh"
#include "mctrigonom.hh"

#include <iostream>
#include <cmath>
#include <cstdlib>

MCGame * MCGameImpl::m_instance = nullptr;

MCGameImpl::MCGameImpl() :
  m_fpsScene(0),
  m_fpsScreen(0),
  m_fpsSceneDelay_ms(0),
  m_fpsScreenDelay_ms(0),
  m_fpsSceneDelay_s(0),
  m_fpsScreenDelay_s(0),
  m_sceneW(0),
  m_sceneH(0),
  m_ticksSinceMainLoopStart(0),
  m_ticksSinceLastRender(0),
  m_bDropFrame(false),
  m_pTM(new MCTextureManager),
  m_pSM(new MCSoundManager),
  m_pWorld(new MCWorld),
  m_pScreen(nullptr),
  m_isSDLInited(false)
{}

MCGameImpl::~MCGameImpl()
{
  if (m_isSDLInited)
  {
    // Close the audio subsystem
    Mix_CloseAudio();

    if (m_pScreen)
    {
      // Delete screen surface
      SDL_FreeSurface(m_pScreen);
      m_pScreen = nullptr;

      // Show cursor again. Not sure if actually needed.
      SDL_ShowCursor(SDL_ENABLE);

      // Quit SDL
      SDL_Quit();
    }
  }
}

MCGame::MCGame() :
  m_pImpl(new MCGameImpl)
{
  if (!MCGameImpl::m_instance) {
    MCGameImpl::m_instance = this;
  } else {
    std::cerr << "ERROR!!: MCGame already instantiated!" << std::endl;
    exit(-1);
  }

  // Init random generator
  std::srand(time(nullptr));
}

MCGame * MCGame::instance() {
  return MCGameImpl::m_instance;
}

MCGame::~MCGame()
{}

MCTextureManager * MCGame::textureManager()
{
  if (instance()) {
    return instance()->m_pImpl->m_pTM.get();
  } else {
    return nullptr;
  }
}

MCSoundManager * MCGame::soundManager()
{
  if (instance()) {
    return instance()->m_pImpl->m_pSM.get();
  } else {
    return nullptr;
  }
}

void MCGame::updateScene()
{
  // Update scene and physics
  m_pImpl->m_pWorld->stepTime(m_pImpl->m_fpsSceneDelay_s);
}

void MCGame::renderCamera(MCCamera * camera)
{
  m_pImpl->m_pWorld->renderShadows(camera);
  m_pImpl->m_pWorld->render(camera);
}

MCWorld * MCGame::world() const
{
  return m_pImpl->m_pWorld.get();
}

bool MCGame::SDLInit(MCUint flags)
{
  // Init SDL with VIDEO and AUDIO flags set by default
  if (SDL_Init(flags) != 0) {
    return false;
  }

  // Get keystate vector
  m_pImpl->m_keystate    = SDL_GetKeyState(nullptr);
  m_pImpl->m_isSDLInited = true;
  return true;
}

bool MCGame::SDLInitDisplay(int w, int h, int bpp, bool bFullScreen, const QString & strCaption)
{
  if (m_pImpl->m_isSDLInited)
  {
    m_pImpl->m_pScreen = nullptr;

    if (bFullScreen)
    {
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      m_pImpl->m_pScreen = SDL_SetVideoMode(w, h, bpp, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN | SDL_OPENGL);
    }
    else
    {
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      m_pImpl->m_pScreen = SDL_SetVideoMode(w, h, bpp, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL);
    }

    if (m_pImpl->m_pScreen)
    {
      SDL_ShowCursor(SDL_DISABLE);
      SDL_WM_SetCaption(strCaption.toStdString().c_str(), strCaption.toStdString().c_str());

      return true;
    }

    return false;
  }
  else
  {
    std::cerr << "ERROR!!: SDL not inited!" << std::endl;
    return false;
  }
}

SDL_Surface * MCGame::SDLScreen() const
{
  return m_pImpl->m_pScreen;
}

bool MCGame::SDLInitAudio(int sampleRate, int numChannels)
{
  if (m_pImpl->m_isSDLInited) {
    if (Mix_OpenAudio(sampleRate, MIX_DEFAULT_FORMAT, numChannels, 1024) == -1) {
      return false;
    }
    return true;
  } else {
    std::cerr << "ERROR!!: SDL not inited!" << std::endl;
    return false;
  }
}

void MCGame::GFXInit2dScene(int hRes, int vRes, int sceneW, int sceneH, MCFloat viewAngle)
{
  // Store scene size
  m_pImpl->m_sceneW = sceneW;
  m_pImpl->m_sceneH = sceneH;

  glEnable( GL_TEXTURE_2D );
  glClearColor( 0, 0, 0, 0 );
  glViewport(0, 0, hRes, vRes );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  // Note: Y-coordinates are reversed
  //  glOrtho(0, HRES, VRES, 0, -1.0, 1.0);

  // Set perspective
  gluPerspective(viewAngle, static_cast<MCFloat>(m_pImpl->m_sceneW) / m_pImpl->m_sceneH, 0.1, 1000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set eye position so that the scene looks like a pure 2D-scene
  MCFloat eyeZ = m_pImpl->m_sceneH / 2 / std::tan(static_cast<MCFloat>(MCTrigonom::degToRad(viewAngle / 2)));
  gluLookAt(m_pImpl->m_sceneW / 2, m_pImpl->m_sceneH / 2, eyeZ,
            m_pImpl->m_sceneW / 2, m_pImpl->m_sceneH / 2, 0,
            0, 1, 0);
}

void MCGame::GFXEnableLighting(GLfloat ar, GLfloat ag, GLfloat ab, GLfloat aa,
                                GLfloat dr, GLfloat dg, GLfloat db, GLfloat da,
                                GLfloat x,  GLfloat y,  GLfloat z,  GLfloat a)
{
  // Create light components
  const GLfloat ambientColor[]  = { ar, ag, ab, aa };
  const GLfloat diffuseColor[]  = { dr, dg, db, da };
  const GLfloat position[]      = { x,  y,  z,  a  };

  // Assign created components to GL_LIGHT1
  glLightfv(GL_LIGHT1, GL_AMBIENT,  ambientColor);
  glLightfv(GL_LIGHT1, GL_DIFFUSE,  diffuseColor);
  glLightfv(GL_LIGHT1, GL_POSITION, position);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT1);
}

void MCGame::GFXDisableLighting()
{
  glDisable(GL_LIGHTING);
}

void MCGame::GFXSetGlobalAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
  GLfloat globalAmbientColor[] = { r, g, b, a };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientColor);
}

void MCGame::GFXEnableColorTracking()
{
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void MCGame::GFXDisableColorTracking()
{
  glDisable(GL_COLOR_MATERIAL);
}

void MCGame::setTargetFps(int fpsScene, int fpsScreen)
{
  // Make sure fps' cannot be less than 1
  fpsScene  = fpsScene  < 1 ? 1 : fpsScene;
  fpsScreen = fpsScreen < 1 ? 1 : fpsScreen;
  m_pImpl->m_fpsScene  = fpsScene;
  m_pImpl->m_fpsScreen = fpsScreen;

  // Nominal delay for the Game loop
  m_pImpl->m_fpsSceneDelay_ms = 1000.0 / fpsScene;
  m_pImpl->m_fpsSceneDelay_s  = 1.0 / fpsScene;

  // Nominal delay for rendering
  m_pImpl->m_fpsScreenDelay_ms = 1000.0 / fpsScreen;
  m_pImpl->m_fpsScreenDelay_s  = 1.0 / fpsScreen;
}

int MCGame::sceneFps() const
{
  return m_pImpl->m_fpsScene;
}

int MCGame::screenFps() const
{
  return m_pImpl->m_fpsScreen;
}

bool MCGame::isTimeToRender() const
{
  // Check if it is time to render (enough time elapsed since
  // previous render)
  bool bTimeToRender = false;
  int delta = m_pImpl->m_ticksSinceMainLoopStart -
              m_pImpl->m_ticksSinceLastRender;
  if (delta > m_pImpl->m_fpsScreenDelay_ms - (TO_INT(SDL_GetTicks()) -
                                              m_pImpl->m_ticksSinceMainLoopStart))
  {
    bTimeToRender = true;
  }
  
  if (!m_pImpl->m_bDropFrame && bTimeToRender)
  {
    // OK to render
    m_pImpl->m_ticksSinceLastRender = SDL_GetTicks();
    return true;
  }

  return false;
}

void MCGame::beginLoopTiming()
{
  m_pImpl->beginLoopTiming();
}

void MCGame::adjustSceneFps()
{
  m_pImpl->adjustSceneFps();
}

void MCGameImpl::beginLoopTiming()
{
  m_ticksSinceMainLoopStart = SDL_GetTicks();
}

void MCGameImpl::adjustSceneFps()
{
  int ticks1;
  ticks1 = SDL_GetTicks();
  ticks1 = ticks1 - m_ticksSinceMainLoopStart;

  if (ticks1 < 0)
  {
    ticks1 = 0;
  }

  if (ticks1 <= m_fpsSceneDelay_ms)
  {
    SDL_Delay(m_fpsSceneDelay_ms - ticks1);
    m_bDropFrame = false;
  }
  else
  {
    // Frame must be dropped
    m_bDropFrame = true;
  }
}

int MCGame::GFXSceneWidth() const
{
  return m_pImpl->m_sceneW;
}

int MCGame::GFXSceneHeight() const
{
  return m_pImpl->m_sceneH;
}

void MCGame::SDLSetWindowIcon(SDL_Surface * p)
{
  SDL_WM_SetIcon(p, nullptr);
}

unsigned char * MCGame::SDLKeystate() const
{
  return m_pImpl->m_keystate;
}

bool MCGame::SDLProcessEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      return false;
    default:
      break;
    }
  }
  return true;
}

void MCGame::GFXSwapBuffers()
{
    SDL_GL_SwapBuffers();
}
