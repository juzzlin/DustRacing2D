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

#include "mcobject.hh"
#include "mcobjectimpl.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mccircleshape.hh"
#include "mcrectshape.hh"
#include "mcsurfaceview.hh"
#include "mcsurface.hh"
#include "mcworld.hh"
#include "mcevent.hh"
#include "mccollisionevent.hh"
#include "mcoutofboundariesevent.hh"
#include "mccamera.hh"
#include "mctrigonom.hh"

UINT MCObjectImpl::m_typeIDCount = 1;
MCObjectImpl::TypeHash MCObjectImpl::m_typeHash;

namespace {
  // Object is stationary
  const UINT STATIONARY_MASK = (1<<0);

  // Object is renderable
  const UINT RENDERABLE_MASK = (1<<1);

  // Object is considered in physics calculations
  const UINT PHYSICS_MASK = (1<<2);

  // Object is considered in collision calculations
  const UINT COLLISIONS_MASK = (1<<3);

  // Object has a shadow
  const UINT SHADOW_MASK = (1<<4);

  // Object is scheduled to be removed from the world
  const UINT REMOVING_MASK = (1<<5);

  // Physics damping factor
  const MCFloat DAMPING_FACTOR = 0.995;
}

MCObjectImpl::MCObjectImpl(MCObject * pPublic, const QString & typeId) :
  m_pPublic(pPublic),
  m_typeID(registerType(typeId)),
  m_time(0),
  m_invMass(std::numeric_limits<MCFloat>::max()),
  m_mass(0),
  m_restitution(0.0),
  m_angle(0),
  m_maximumVelocity(-1),
  m_layer(0),
  m_index(-1),
  m_flags(RENDERABLE_MASK | PHYSICS_MASK | COLLISIONS_MASK | SHADOW_MASK),
  m_i0(0), m_i1(0), m_j0(0), m_j1(0),
  m_pShape(NULL)
{}

void MCObjectImpl::setFlag(UINT flag, bool enable)
{
  if (enable) {
    m_flags |= flag;
  } else {
    m_flags &= ~flag;
  }
}

UINT MCObjectImpl::typeID(const QString & typeName)
{
  const TypeHash::iterator i(m_typeHash.find(typeName));
  if (i == m_typeHash.end()) {
    return 0;
  } else {
    return i.value();
  }
}

UINT MCObjectImpl::registerType(const QString & typeName)
{
  const TypeHash::iterator i(m_typeHash.find(typeName));
  if (i == m_typeHash.end()) {
    m_typeIDCount++;
    m_typeHash[typeName] = m_typeIDCount;
    return m_typeIDCount;
  } else {
    return i.value();
  }
}

void MCObjectImpl::integrate(MCFloat step)
{
  if (step > 0.0) {

    MCVector3d<MCFloat> acceleration(m_acceleration);
    m_pPublic->translate(m_location + m_velocity * step);
    acceleration += m_forces * m_invMass;
    m_velocity   += acceleration * step;
    m_velocity   *= DAMPING_FACTOR;

    // Note that this code doesn't take the z-component into consideration
    if (m_maximumVelocity > 0) {
      const MCFloat l = MCVector2d<MCFloat>(m_velocity).lengthFast();
      if (l > m_maximumVelocity) {
        m_velocity /= l;
        m_velocity *= m_maximumVelocity;
      }
    }

    m_forces.setZero();
    doOutOfBoundariesEvent();
  }
}

void MCObjectImpl::doOutOfBoundariesEvent()
{
  const MCWorld * pWorld = MCWorld::instance();
  if (m_location.i() < pWorld->minX()) {
    m_pPublic->translate(MCVector3d<MCFloat>(pWorld->minX(), m_location.j(), m_location.k()));
    MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::West);
    m_pPublic->outOfBoundariesEvent(&e);
  } else if (m_location.i() > pWorld->maxX()) {
    m_pPublic->translate(MCVector3d<MCFloat>(pWorld->maxX(), m_location.j(), m_location.k()));
    MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::East);
    m_pPublic->outOfBoundariesEvent(&e);
  }

  if (m_location.j() < pWorld->minY()) {
    m_pPublic->translate(MCVector3d<MCFloat>(m_location.i(), pWorld->minY(), m_location.k()));
    MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::South);
    m_pPublic->outOfBoundariesEvent(&e);
  } else if (m_location.j() > pWorld->maxY()) {
    m_pPublic->translate(MCVector3d<MCFloat>(m_location.i(), pWorld->maxY(), m_location.k()));
    MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::North);
    m_pPublic->outOfBoundariesEvent(&e);
  }

  if (m_location.k() < pWorld->minZ()) {
    m_pPublic->translate(MCVector3d<MCFloat>(m_location.i(), m_location.j(), pWorld->minZ()));
    MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Bottom);
    m_pPublic->outOfBoundariesEvent(&e);
  } else if (m_location.k() > pWorld->maxZ()) {
    m_pPublic->translate(MCVector3d<MCFloat>(m_location.i(), m_location.j(), pWorld->maxZ()));
    MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Top);
    m_pPublic->outOfBoundariesEvent(&e);
  }
}

MCObjectImpl::~MCObjectImpl()
{}

MCObject::MCObject(const QString & typeId) :
    m_pImpl(new MCObjectImpl(this, typeId))
{}

MCObject::MCObject(MCShape * pShape, const QString & typeId) :
    m_pImpl(new MCObjectImpl(this, typeId))
{
  m_pImpl->m_pShape = pShape;
}

MCObject::MCObject(MCSurface * pSurface, const QString & typeId) :
    m_pImpl(new MCObjectImpl(this, typeId))
{
  // Create an MCRectShape using pSurface with an MCSurfaceView
  MCRectShape * rectShape = new MCRectShape(this,
                                            new MCSurfaceView(pSurface),
                                            pSurface ? pSurface->width() : 0,
                                            pSurface ? pSurface->height() : 0);
  m_pImpl->m_pShape = rectShape;
}

void MCObject::resetMotion()
{
  m_pImpl->m_velocity.setZero();
  m_pImpl->m_forces.setZero();
}

void MCObject::setSurface(MCSurface * pSurface)
{
  if (m_pImpl->m_pShape && m_pImpl->m_pShape->view()) {
    MCSurfaceView * pView = dynamic_cast<MCSurfaceView *>(m_pImpl->m_pShape->view());
    if (pView) {
      if (pView->surface() != pSurface) {
        pView->setSurface(pSurface);
      }
    }
  }
}

MCSurface * MCObject::surface() const
{
  if (m_pImpl->m_pShape && m_pImpl->m_pShape->view()) {
    MCSurfaceView * pView = dynamic_cast<MCSurfaceView *>(m_pImpl->m_pShape->view());
    if (pView) {
      return pView->surface();
    }
  }
  return NULL;
}

UINT MCObject::typeID() const
{
  return m_pImpl->m_typeID;
}

UINT MCObject::typeID(const QString & typeName)
{
  return MCObjectImpl::typeID(typeName);
}

UINT MCObject::registerType(const QString & typeName)
{
  return MCObjectImpl::registerType(typeName);
}

bool MCObject::event(MCEvent * pEvent)
{
  if (pEvent->instanceTypeID() == MCCollisionEvent::typeID())
  {
    collisionEvent(static_cast<MCCollisionEvent *>(pEvent));
    return true;
  }
  else if (pEvent->instanceTypeID() == MCOutOfBoundariesEvent::typeID()) {
    outOfBoundariesEvent(static_cast<MCOutOfBoundariesEvent *>(pEvent));
    return true;
  }

  return false;
}

void MCObject::collisionEvent(MCCollisionEvent * e)
{
  e->ignore();
}

void MCObject::outOfBoundariesEvent(MCOutOfBoundariesEvent * e)
{
  e->ignore();
}

void MCObject::sendEvent(MCObject * pObject, MCEvent * pEvent)
{
  pObject->event(pEvent);
}

void MCObject::addToWorld()
{
  if (MCWorld::instance()) {
    MCWorld::instance()->addObject(this);
  }
}

void MCObject::removeFromWorld()
{
  if (MCWorld::instance()) {
    MCWorld::instance()->removeObject(this);
  }
}

void MCObject::removeFromWorldNow()
{
  if (MCWorld::instance()) {
    MCWorld::instance()->removeObjectNow(this);
  }
}

void MCObject::render(MCCamera * p)
{
  if (m_pImpl->m_pShape) {
    m_pImpl->m_pShape->render(p);
  }
}

void MCObject::renderShadow(MCCamera * p)
{
  if (m_pImpl->m_pShape) {
    m_pImpl->m_pShape->renderShadow(p);
  }
}

void MCObject::setMass(MCFloat newMass, bool stationary_)
{
  m_pImpl->setFlag(STATIONARY_MASK, stationary_);

  if (!stationary_) {
    m_pImpl->m_invMass = 1.0 / newMass;
    m_pImpl->m_mass    = 0;
  }
  else {
    m_pImpl->m_invMass = 0;
    m_pImpl->m_mass    = std::numeric_limits<MCFloat>::max();
  }
}

MCFloat MCObject::invMass() const
{
  return m_pImpl->m_invMass;
}

MCFloat MCObject::mass() const
{
  return m_pImpl->m_mass;
}

bool MCObject::stationary() const
{
  return m_pImpl->m_flags & STATIONARY_MASK;
}

void MCObject::addImpulse(const MCVector3d<MCFloat> & impulse)
{
  m_pImpl->m_velocity += impulse;
}

void MCObject::setPhysicsObject(bool flag)
{
  m_pImpl->setFlag(PHYSICS_MASK, flag);
}

bool MCObject::physicsObject() const
{
  return m_pImpl->m_flags & PHYSICS_MASK;
}

void MCObject::setBypassCollisions(bool flag)
{
  m_pImpl->setFlag(COLLISIONS_MASK, !flag);
}

bool MCObject::bypassCollisions() const
{
  return !(m_pImpl->m_flags & COLLISIONS_MASK);
}

void MCObject::setRenderable(bool flag)
{
  m_pImpl->setFlag(RENDERABLE_MASK, flag);
}

bool MCObject::renderable() const
{
  return m_pImpl->m_flags & RENDERABLE_MASK;
}

void MCObject::setHasShadow(bool flag)
{
  m_pImpl->setFlag(SHADOW_MASK, flag);
}

bool MCObject::hasShadow() const
{
  return m_pImpl->m_flags & SHADOW_MASK;
}

void MCObject::setMaximumVelocity(MCFloat maxVelocity)
{
  m_pImpl->m_maximumVelocity = maxVelocity;
}

void MCObject::setVelocity(const MCVector3d<MCFloat> & newVelocity)
{
  m_pImpl->m_velocity = newVelocity;
}

const MCVector3d<MCFloat> & MCObject::velocity() const
{
  return m_pImpl->m_velocity;
}

void MCObject::setAcceleration(const MCVector3d<MCFloat> & newAcceleration)
{
  m_pImpl->m_acceleration = newAcceleration;
}

const MCVector3d<MCFloat> & MCObject::acceleration() const
{
  return m_pImpl->m_acceleration;
}

void MCObject::translate(const MCVector3d<MCFloat> & newLocation)
{
  const bool wasInWorld = !removing() && MCWorld::instance()->tree()->remove(this);
  m_pImpl->m_location = newLocation;
  if (m_pImpl->m_pShape) {
    m_pImpl->m_pShape->translate(newLocation);
  }
  if (wasInWorld) {
    MCWorld::instance()->tree()->insert(this);
  }
}

void MCObject::displace(const MCVector3d<MCFloat> & displacement)
{
  translate(m_pImpl->m_location + displacement);
}

const MCVector3d<MCFloat> & MCObject::location() const
{
  return m_pImpl->m_location;
}

void MCObject::setShadowOffset(const MCVector2d<MCFloat> & p)
{
  if (m_pImpl->m_pShape) {
    m_pImpl->m_pShape->setShadowOffset(p);
  }
}

MCFloat MCObject::getX() const
{
  return m_pImpl->m_location.i();
}

MCFloat MCObject::getY() const
{
  return m_pImpl->m_location.j();
}

MCFloat MCObject::getZ() const
{
  return m_pImpl->m_location.k();
}

void MCObject::rotate(UINT newAngle)
{ 
  if (newAngle != m_pImpl->m_angle) {

    const UINT MAX_ANGLE = 360;
    newAngle %= MAX_ANGLE;
    m_pImpl->m_angle = newAngle;

    if (m_pImpl->m_pShape) {
      if (dynamic_cast<MCCircleShape *>(m_pImpl->m_pShape)) {
        m_pImpl->m_pShape->rotate(m_pImpl->m_angle);
      } else {
        const bool wasInWorld = MCWorld::instance()->tree()->remove(this);
        m_pImpl->m_pShape->rotate(m_pImpl->m_angle);
        if (wasInWorld) {
          MCWorld::instance()->tree()->insert(this);
        }
      }
    }
  }
}

UINT MCObject::angle() const
{
  return m_pImpl->m_angle;
}

MCVector2d<MCFloat> MCObject::direction() const
{
  return MCVector2d<MCFloat>(MCTrigonom::cos(angle()), MCTrigonom::sin(angle()));
}

void MCObject::setRestitution(MCFloat newRestitution)
{
  newRestitution = newRestitution < 0.0 ? 0.0 : newRestitution;
  newRestitution = newRestitution > 1.0 ? 1.0 : newRestitution;
  m_pImpl->m_restitution = newRestitution;
}

MCFloat MCObject::restitution() const
{
  return m_pImpl->m_restitution;
}

void MCObject::setShape(MCShape * newShape)
{
  if (m_pImpl->m_pShape) {
    delete m_pImpl->m_pShape;
  }

  m_pImpl->m_pShape = newShape;
}

MCShape * MCObject::shape() const
{
  return m_pImpl->m_pShape;
}

void MCObject::setView(MCShapeView * newView)
{
  if (m_pImpl->m_pShape) {
    m_pImpl->m_pShape->setView(newView);
  }
}

MCShapeView * MCObject::view() const
{
  if (m_pImpl->m_pShape) {
    return m_pImpl->m_pShape->view();
  } else {
    return NULL;
  }
}

void MCObject::addForce(const MCVector3d<MCFloat> & force)
{
  m_pImpl->m_forces += force;
}

void MCObject::clearForces()
{
  m_pImpl->m_forces.setZero();
}

void MCObject::integrate(MCFloat step)
{
  m_pImpl->integrate(step);
  translate(location() + velocity());
}

MCBBox<MCFloat> MCObject::bbox() const
{
  if (m_pImpl->m_pShape) {
    return m_pImpl->m_pShape->bbox();
  } else {
    return MCBBox<MCFloat>(getX(), getY(), getX() + 1, getY() + 1);
  }
}

void MCObject::stepTime()
{
  m_pImpl->m_time++;
}

UINT MCObject::time() const
{
  return m_pImpl->m_time;
}

void MCObject::resetTime()
{
  m_pImpl->m_time = 0;
}

void MCObject::setLayer(UINT newLayer)
{
  MCWorld::instance()->removeFromLayerMap(this);
  m_pImpl->m_layer = newLayer;
  MCWorld::instance()->addToLayerMap(this);
}

UINT MCObject::layer() const
{
  return m_pImpl->m_layer;
}

void MCObject::setIndex(int newIndex)
{
  m_pImpl->m_index = newIndex;
}

int MCObject::index() const
{
  return m_pImpl->m_index;
}

void MCObject::cacheIndexRange(UINT i0, UINT i1, UINT j0, UINT j1)
{
  m_pImpl->m_i0 = i0;
  m_pImpl->m_i1 = i1;
  m_pImpl->m_j0 = j0;
  m_pImpl->m_j1 = j1;
}

void MCObject::restoreIndexRange(UINT * i0, UINT * i1, UINT * j0, UINT * j1)
{
  *i0 = m_pImpl->m_i0;
  *i1 = m_pImpl->m_i1;
  *j0 = m_pImpl->m_j0;
  *j1 = m_pImpl->m_j1;
}

void MCObject::setRemoving(bool flag)
{
  m_pImpl->setFlag(REMOVING_MASK, flag);
}

bool MCObject::removing() const
{
  return m_pImpl->m_flags & REMOVING_MASK;
}

void MCObject::addContact(MCContact * contact)
{
  m_pImpl->m_contacts[contact->object()] << contact;
}

const MCObject::ContactHash & MCObject::contacts() const
{
  return m_pImpl->m_contacts;
}

void MCObject::deleteContacts()
{
  ContactHash::iterator i(m_pImpl->m_contacts.begin());
  for (; i != m_pImpl->m_contacts.end(); i++) {
    for (int j = 0; j < i.value().size(); j++) {
      i.value()[j]->free();
    }
  }
  m_pImpl->m_contacts.clear();
}

void MCObject::deleteContacts(MCObject * p)
{
  ContactHash::iterator i(m_pImpl->m_contacts.find(p));
  if (i != m_pImpl->m_contacts.end()) {
    for (int j = 0; j < i.value().size(); j++) {
      i.value()[j]->free();
    }
    i.value().clear();
  }
}

MCObject::~MCObject()
{
  deleteContacts();

  delete m_pImpl->m_pShape;
  delete m_pImpl;
}
