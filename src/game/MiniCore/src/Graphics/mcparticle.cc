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

#include "mcparticle.hh"
#include "mccircleshape.hh"

int MCParticle::m_numActiveParticles = 0;

MCParticle::MCParticle(const std::string & typeID)
: MCObject(typeID)
, m_lifeTime(0)
, m_initLifeTime(0)
, m_animationStyle(MCParticle::AnimationStyle::None)
, m_isActive(false)
, m_dieWhenOffScreen(true)
, m_dieOnOutOfBoundariesEvent(true)
, m_freeList(nullptr)
, m_customDeathCondition(nullptr)
{
    setShape(MCShapePtr(new MCCircleShape(nullptr, 0.0)));
    setBypassCollisions(true);
    setIsPhysicsObject(true);
    setIsParticle(true);
}

MCParticle::~MCParticle()
{
}

void MCParticle::init(MCVector3dFR newLocation, MCFloat newRadius, MCUint newLifeTime)
{
    m_lifeTime       = newLifeTime;
    m_initLifeTime   = newLifeTime;
    m_isActive       = true;
    m_radius         = newRadius;
    m_scale          = 1.0f;

    setIsPhysicsObject(true);

    translate(newLocation);

    MCParticle::m_numActiveParticles++;
}

void MCParticle::setFreeList(ParticleFreeList & freeList)
{
    m_freeList = &freeList;
}

MCFloat MCParticle::radius() const
{
    if (animationStyle() == MCParticle::AnimationStyle::Shrink)
    {
        return m_scale * m_radius;
    }
    else if (animationStyle() == MCParticle::AnimationStyle::FadeOutAndExpand)
    {
        return (2.0f - m_scale) * m_radius;
    }
    else
    {
        return m_radius;
    }
}

MCUint MCParticle::lifeTime() const
{
    return m_lifeTime;
}

MCUint MCParticle::initLifeTime() const
{
    return m_initLifeTime;
}

void MCParticle::setAnimationStyle(MCParticle::AnimationStyle style)
{
    m_animationStyle = style;
}

MCParticle::AnimationStyle MCParticle::animationStyle() const
{
    return m_animationStyle;
}

void MCParticle::setDieWhenOffScreen(bool flag)
{
    m_dieWhenOffScreen = flag;
}

bool MCParticle::dieWhenOffScreen() const
{
    return m_dieWhenOffScreen;
}

void MCParticle::setDieOnOutOfBoundariesEvent(bool flag)
{
    m_dieOnOutOfBoundariesEvent = flag;
}

bool MCParticle::dieOnOutOfBoundariesEvent() const
{
    return m_dieOnOutOfBoundariesEvent;
}

void MCParticle::onStepTime(int step)
{
    if (m_lifeTime >= step)
    {
        m_lifeTime -= step;

        m_scale = MCFloat(m_lifeTime) / m_initLifeTime;

        if (m_customDeathCondition && m_customDeathCondition(*this))
        {
            m_lifeTime = 0;
        }
    }
    else
    {
        m_lifeTime = 0;

        timeOut();
    }
}

void MCParticle::outOfBoundariesEvent(MCOutOfBoundariesEvent &)
{
    if (m_dieOnOutOfBoundariesEvent)
    {
        die();
    }
}

MCFloat MCParticle::scale() const
{
    return m_scale;
}

void MCParticle::timeOut()
{
    die();
}

bool MCParticle::isActive() const
{
    return m_isActive;
}

void MCParticle::die()
{
    if (m_isActive)
    {
        m_isActive = false;

        MCParticle::m_numActiveParticles--;

        removeFromWorld();

        if (m_freeList)
        {
            m_freeList->push_back(this);
        }
    }
}

int MCParticle::numActiveParticles()
{
    return MCParticle::m_numActiveParticles;
}

void MCParticle::setCustomDeathCondition(CustomDeathConditionFunction customDeathCondition)
{
    m_customDeathCondition = customDeathCondition;
}
