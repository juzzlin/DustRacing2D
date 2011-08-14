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

#ifndef MCGAME_HH
#define MCGAME_HH

#include <QString>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_mixer.h>

#include <tr1/memory>

using std::tr1::shared_ptr;

#include "mcmacros.hh"
#include "mctypes.hh"

class MCTextureManager;
class MCSoundManager;
class MCWorld;
class MCCamera;
class MCGameImpl;

class SDL_Surface;

/*! Abstract base class for SDL/OpenGL (2d) -games.
 *
 * This class provides:
 *
 * - methods for initializing graphics and audio
 * - sound manager to load Sound-objects to play
 * - texture manager to load and manage textures
 * - game clocking
 *
 * All methods are prefixed with SDL, GFX, SFX and CLK:
 *
 * SDL: an SDL method
 * SFX: a sounds-related method
 * GFX: a graphics-related method
 *
 * All methods have a default implementation and most can be
 * re-implemented to get the desired functionality.
 */
class MCGame
{
public:

  //! Constructor
  MCGame();

  //! Destructor
  virtual ~MCGame();

  //! \brief This starts the main loop.
  virtual void run() = 0;

  //! \brief Return the TextureManager.
  static MCTextureManager * textureManager();

  //! \brief Return the SoundManager.
  static MCSoundManager * soundManager();

  //! \brief Return the one-and-only instance.
  static MCGame * instance();

  //! Return scene Fps
  int sceneFps() const;

  //! Return screen Fps
  int screenFps() const;

  //! Get scene window width.
  int GFXSceneWidth() const;

  //! Get scene window height.
  int GFXSceneHeight() const;

protected:

  /*! \brief Main loop.
   * The main loop could go like this in a game inherited from MCGame:
   *
   * \code
   *  setTargetFps(50, 50);
   *
   *  while(gameActive)
   *  {
   *    beginLoopTiming();
   *
   *    // Process controls, events etc here
   *    ...
   *
   *    // Update scene, physics and timing
   *    updateScene();
   *
   *    // Check if we can render
   *    if (isTimeToRender()) {
   *
   *      // Render all renderable objects added to the world
   *      renderCamera(currentCamera);
   *
   *      // Render your custom stuff here
   *      ...
   *    }
   *
   *    adjustSceneFps();
   *  }
   * \endcode
   */
  virtual void mainLoop() = 0;

  /*! \brief Update scene, physics and timing.
   */
  virtual void updateScene();

  /*! \brief Render the World as seen through pCamera.
   * Base class implementation must be called if overridden.
   */
  virtual void renderCamera(MCCamera * pCamera);

  //! Return pointer to the MCWorld object
  MCWorld * world() const;

  /*! \brief Init SDL.
   * This must be called successfully before anything else.
   * \param flags Flags to SDL_Init.
   */
  virtual bool SDLInit(UINT flags = (SDL_INIT_VIDEO | SDL_INIT_AUDIO));

  //! \brief Create an SDL window with given parameters.
  virtual bool SDLInitDisplay(int w, int h, int bpp, bool bFullScreen, const QString & strCaption);

  //! \brief Initialize SDL_Mixer.
  virtual bool SDLInitAudio(int sampleRate = 44100, int numChannels = 2);

  /*! \brief Process SDL events, return false on quit.
   * The keystate vector can be updated by calling this.
   * \return false On system exit.
   */
  virtual bool SDLProcessEvents();

  //! \brief Get the SDL keystate vector.
  unsigned char * SDLKeystate() const;

  //! \brief Get the screen surface.
  SDL_Surface * SDLScreen() const;

  //! \brief  Sets the window icon. SDL must be inited.
  virtual void SDLSetWindowIcon(SDL_Surface * p);

  /*! \brief  Sets up a 2d-scene using OpenGL.
   * Positive Z is "up" in the default impl.
   */
  virtual void GFXInit2dScene(int hRes, int vRes, int sceneWidth, int sceneHeight, MCFloat viewAngle);

  //! Enable a simple lighting.
  virtual void GFXEnableLighting(GLfloat ar, GLfloat ag, GLfloat ab, GLfloat aa,
                                 GLfloat dr, GLfloat dg, GLfloat db, GLfloat da,
                                 GLfloat x,  GLfloat y,  GLfloat z,  GLfloat a);

  //! Disable lighting.
  virtual void GFXDisableLighting();

  //! Set global ambient light.
  virtual void GFXSetGlobalAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

  //! Enable material color tracking.
  virtual void GFXEnableColorTracking();

  //! Disable material color tracking.
  virtual void GFXDisableColorTracking();

  //! Swap buffers.
  virtual void GFXSwapBuffers();

  //! Set target Fps's. sceneFps is independent of screen(rendering)Fps.
  virtual void setTargetFps(int sceneFps = 50, int screenFps = 50);

  //! Check if it's time to render according to the timing scheme set by setTargetFps().
  virtual bool isTimeToRender() const;

  //!
  void beginLoopTiming();

  //!
  void adjustSceneFps();

private:

  //! Disable copy constructor and assignment
  DISABLE_COPY(MCGame);
  DISABLE_ASSI(MCGame);
  shared_ptr<MCGameImpl> const m_pImpl;
};

#endif // MCGAME_HH
