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

#ifndef MCGAMEIMPL_HH
#define MCGAMEIMPL_HH

//! Implementation class for MCGame
class MCGameImpl
{
public:
  MCGameImpl();
  virtual ~MCGameImpl();

private:

  //! Adjust Fps delay of the main loop.
  void adjustSceneFps();

  //! Begin clocking the main loop
  void beginLoopTiming();

  static MCGame * m_instance;
  int m_fpsScene;
  int m_fpsScreen;
  int m_fpsSceneDelay_ms;
  int m_fpsScreenDelay_ms;
  MCFloat m_fpsSceneDelay_s;
  MCFloat m_fpsScreenDelay_s;
  int m_sceneW;
  int m_sceneH;
  mutable int m_ticksSinceMainLoopStart;
  mutable int m_ticksSinceLastRender;
  bool m_bDropFrame;
  shared_ptr<MCTextureManager> m_pTM;
  shared_ptr<MCSoundManager> m_pSM;
  shared_ptr<MCWorld> m_pWorld;
  SDL_Surface * m_pScreen;
  unsigned char * m_keystate;
  bool m_isSDLInited;
  friend class MCGame;
};

#endif // MCGAMEIMPL_HH
