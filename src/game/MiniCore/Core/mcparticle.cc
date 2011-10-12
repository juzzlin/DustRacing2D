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

#include "mcparticle.hh"
#include "mcparticleimpl.hh"
#include "mccircleshape.hh"
#include "mccast.hh"

MCRecycler<MCParticle> MCParticle::m_recycler;

MCParticleImpl::MCParticleImpl() :
    m_lifeTime(0),
    m_initLifeTime(0),
    m_animationStyle(MCParticle::None),
    m_isActive(false),
    m_scale(1.0)
{}

MCParticleImpl::~MCParticleImpl()
{}

MCParticle::MCParticle() :
    MCObject("PARTICLE"),
    m_pImpl(new MCParticleImpl)
{
  m_pImpl->m_pPublic = this;

  setShape(new MCCircleShape(this, nullptr, 0.0f));
  setBypassCollisions(true);
}

MCParticle::~MCParticle()
{
  delete m_pImpl;
}

void MCParticle::init(const MCVector3d<MCFloat> & newLocation, MCFloat newRadius, UINT newLifeTime)
{
  m_pImpl->m_lifeTime       = newLifeTime;
  m_pImpl->m_initLifeTime   = newLifeTime;
  m_pImpl->m_animationStyle = MCParticle::None;
  m_pImpl->m_isActive       = true;
  m_pImpl->m_scale          = 1.0;

  static_cast<MCCircleShape *>(shape())->setRadius(newRadius);

  translate(newLocation);
}

MCFloat MCParticle::radius() const
{
  return static_cast<MCCircleShape *>(shape())->radius();
}

UINT MCParticle::lifeTime() const
{
  return m_pImpl->m_lifeTime;
}

UINT MCParticle::initLifeTime() const
{
  return m_pImpl->m_initLifeTime;
}

void MCParticle::setAnimationStyle(MCParticle::AnimationStyle style)
{
  m_pImpl->m_animationStyle = style;
}

MCParticle::AnimationStyle MCParticle::animationStyle() const
{
  return m_pImpl->m_animationStyle;
}

void MCParticle::stepTime()
{
  m_pImpl->stepTime();
}

void MCParticleImpl::stepTime()
{
  if (m_lifeTime) {
    m_lifeTime--;
    m_scale = static_cast<MCFloat>(m_lifeTime) / m_initLifeTime;
  } else if (m_isActive) {
    m_isActive = false;
    m_pPublic->timeOut();
  }
}

MCFloat MCParticle::scale() const
{
  return m_pImpl->m_scale;
}

void MCParticle::timeOut()
{
  die();
}

void MCParticle::render(MCCamera * pCamera)
{
  if (animationStyle() == MCParticle::Shrink) {
    const MCFloat s = scale() / 2.0;
    shape()->renderScaled(s * bbox().width(), s * bbox().height(), pCamera);
  } else {
    MCObject::render(pCamera);
  }
}

void MCParticle::renderShadow(MCCamera * pCamera)
{
  if (animationStyle() == MCParticle::Shrink) {
    const MCFloat s = scale() / 2.0;
    shape()->renderShadowScaled(s * bbox().width(), s * bbox().height(), pCamera);
  } else {
    MCObject::renderShadow(pCamera);
  }
}

MCParticle * MCParticle::create()
{
  return m_recycler.newObject();
}

bool MCParticle::isActive() const
{
  return m_pImpl->m_isActive;
}

void MCParticle::die()
{
  removeFromWorld();
  m_pImpl->m_isActive = false;
  recycle();
}

void MCParticle::recycle()
{
  m_recycler.freeObject(this);
}
