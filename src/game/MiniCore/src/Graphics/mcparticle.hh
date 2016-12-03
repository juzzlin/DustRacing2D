// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
#include "mcworldrenderer.hh"

#include <functional>
#include <vector>

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

    friend class MCWorldRenderer;

    typedef std::vector<MCParticle *> ParticleFreeList;

    //! Style of the disappear animation
    enum class AnimationStyle {
        None,
        Shrink,
        FadeOut,
        FadeOutAndExpand
    };

    //! Constructor.
    MCParticle(const std::string & typeID);

    //! Destructor
    virtual ~MCParticle();

    //! Finish the particle. Automatically called on timeOut().
    void die();

    /*! (Re-)Init a particle.
   *  \param location Initial location.
   *  \param radius   Initial radius.
   *  \param lifeTime Life time / number of frames. The life time is decremented
   *  each time stepTime() is called. */
    void init(MCVector3dFR location, MCFloat radius, MCUint lifeTime);

    //! Particle will be added to the given vector when it dies.
    void setFreeList(ParticleFreeList & freeList);

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
   * the life time. Default is None. */
    void setAnimationStyle(MCParticle::AnimationStyle style);

    //! Return animation style
    MCParticle::AnimationStyle animationStyle() const;

    /*! Optimization: if set to true, the particle is immediately killed if it goes off screen (camera).
   *  Default is true. */
    void setDieWhenOffScreen(bool flag);

    bool dieWhenOffScreen() const;

    /*! If set to true, the particle is immediately killed if it goes off world boundaries.
   *  Default is true. */
    void setDieOnOutOfBoundariesEvent(bool flag);

    bool dieOnOutOfBoundariesEvent() const;

    //! \reimp
    virtual void onStepTime(int step) override;

    //! \reimp
    virtual void outOfBoundariesEvent(MCOutOfBoundariesEvent & event) override;

    //! Get timeline scale from 1.0 to 0.0
    MCFloat scale() const;

    /*! Set custom death condition function.
   *  The function will be called on each update with this as the parameter.
   *  If the function returns true, the particle will be killed. */
    typedef std::function<bool(MCParticle &)> CustomDeathConditionFunction;
    void setCustomDeathCondition(CustomDeathConditionFunction customDeathCondition);

    //! Get number of active particles.
    static int numActiveParticles();

protected:

    /*! This is called when lifeTime reaches zero. Default implementation
   *  removes from the world. Re-imp if desired.
   */
    virtual void timeOut();

private:

    static int m_numActiveParticles;

    DISABLE_COPY(MCParticle);
    DISABLE_ASSI(MCParticle);

    int m_lifeTime;

    int m_initLifeTime;

    MCParticle::AnimationStyle m_animationStyle;

    bool m_isActive;

    bool m_dieWhenOffScreen;

    bool m_dieOnOutOfBoundariesEvent;

    MCFloat m_radius;

    MCFloat m_scale;

    MCParticle::ParticleFreeList * m_freeList;

    CustomDeathConditionFunction m_customDeathCondition;

    int m_indexInRenderArray = -1;
};

#endif // MCPARTICLE_HH
