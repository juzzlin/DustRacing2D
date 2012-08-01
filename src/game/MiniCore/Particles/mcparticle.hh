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

#ifndef MCPARTICLE_HH
#define MCPARTICLE_HH

#include "mcobject.hh"
#include "mcrecycler.hh"

/*! \class MCParticle
 *  \brief Base particle class for particle animations.
 *
 *  Particles always internally use MCCircleShape regardless of their
 *  view. Particle collisions are bypassed (setBypassCollisions(bool)) by default
 *  to significantly speed up animations with huge number of particles.
 */
class MCParticle : public MCObject
{
public:

  //! Style of the disappear animation
  enum AnimationStyle {None = 0, Shrink, FadeOut};

  //! Create a new particle or return recycled one.
  //! Inherited classes must re-define this.
  static MCParticle & create();

  //! Called by die().
  //! Inherited classes must re-define this.
  virtual void recycle();

  //! Finish the particle. Automatically called on timeOut().
  void die();

  //! (Re-)Init a particle.
  //! \param location Initial location.
  //! \param radius   Initial radius.
  //! \param lifeTime Life time / number of frames. The life time is decremented
  //! each time stepTime() is called.
  void init(const MCVector3d<MCFloat> & location, MCFloat radius, MCUint lifeTime);

  //! Return radius
  MCFloat radius() const;

  //! Return life-time. It's decreased on each call to stepTime().
  MCUint lifeTime() const;

  //! Return initial life-time set by init().
  MCUint initLifeTime() const;

  //! Return true if still active
  bool isActive() const;

  /*! \brief Set animation style.
   * This parameter defines the animation performed linearily during
   * the life time. Default is None.
   */
  void setAnimationStyle(MCParticle::AnimationStyle style);

  //! Return animation style
  MCParticle::AnimationStyle animationStyle() const;

  //! \reimp
  virtual void stepTime();

  //! \reimp
  virtual void render(MCCamera * pCamera = nullptr);

  //! \reimp
  virtual void renderShadow(MCCamera * pCamera = nullptr);

  //! Get timeline scale from 1.0 to 0.0
  MCFloat scale() const;

  //! Get number of active particles.
  static int numActiveParticles();

protected:

  //! Constructor
  MCParticle();

  //! Destructor
  virtual ~MCParticle();

  /*! This is called when lifeTime reaches zero. Default implementation
   *  removes from the world. Re-imp if desired.
   */
  virtual void timeOut();

private:

  //! Recycler object
  static MCRecycler<MCParticle> m_recycler;
  friend class MCRecycler<MCParticle>;

  static int m_numActiveParticles;

  DISABLE_COPY(MCParticle);
  DISABLE_ASSI(MCParticle);

  MCUint m_lifeTime;
  MCUint m_initLifeTime;
  MCParticle::AnimationStyle m_animationStyle;
  bool m_isActive;
  MCFloat m_scale;
  MCFloat m_delta;
  MCFloat m_radius;
};

#endif // MCPARTICLE_HH
