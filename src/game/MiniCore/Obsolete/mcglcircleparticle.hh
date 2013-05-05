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

#ifndef MCGLCIRCLEPARTICLE_HH
#define MCGLCIRCLEPARTICLE_HH

#include "mcparticle.hh"

class MCGLCircleParticleImpl;
class MCCamera;

/*! \class MCGLCircleParticle
 *  \brief A particle that renders as a simple OpenGL circle. Final class.
 */
class MCGLCircleParticle : public MCParticle
{
public:

  //! Create a new particle
  static MCGLCircleParticle * create();

  //! \reimp
  void recycle();

  //! Set color
  void setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a = 1.0);

  //! \reimp
  void render(MCCamera * camera = nullptr);

  //! \reimp
  void renderShadow(MCCamera * camera = nullptr);

private:

  //! Constructor
  MCGLCircleParticle();

  //! Destructor
  ~MCGLCircleParticle();

  //! Recycler object
  static MCRecycler<MCGLCircleParticle> m_recycler;
  friend class MCRecycler<MCGLCircleParticle>;

  DISABLE_COPY(MCGLCircleParticle);
  DISABLE_ASSI(MCGLCircleParticle);
  MCGLCircleParticleImpl * const m_pImpl;
};

#endif // MCGLCIRCLEPARTICLE_HH
