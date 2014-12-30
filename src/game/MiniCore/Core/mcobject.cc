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

#include "mccamera.hh"
#include "mccircleshape.hh"
#include "mccollisionevent.hh"
#include "mcevent.hh"
#include "mcoutofboundariesevent.hh"
#include "mcrectshape.hh"
#include "mcshapeview.hh"
#include "mcsurface.hh"
#include "mctimerevent.hh"
#include "mctrigonom.hh"
#include "mcsurfaceview.hh"
#include "mcworld.hh"
#include "mcworldrenderer.hh"

#include <cassert>

MCUint MCObject::m_typeIDCount = 1;
MCObject::TypeHash MCObject::m_typeHash;
MCObject::TimerEventObjectsList MCObject::m_timerEventObjects;

static const MCFloat DAMPING = 0.999f;

MCObject::MCObject(const std::string & typeId)
{
    init(typeId);
}

MCObject::MCObject(MCShapePtr shape, const std::string & typeId)
{
    init(typeId);

    setShape(shape);
}

MCObject::MCObject(MCSurface & surface, const std::string & typeId, bool batchMode)
{
    init(typeId);

    // Create an MCRectShape using surface with an MCSurfaceView
    MCShapePtr rectShape(new MCRectShape(
        MCShapeViewPtr(new MCSurfaceView(typeId, &surface)),
        surface.width(),
        surface.height()));

    rectShape->view()->setBatchMode(batchMode);

    setShape(rectShape);
}

void MCObject::init(const std::string & typeId)
{
    m_typeID                 = registerType(typeId);
    m_invMass                = std::numeric_limits<MCFloat>::max();
    m_mass                   = 0;
    m_restitution            = 0.5;
    m_xyFriction             = 0.0;
    m_angle                  = 0;
    m_relativeAngle          = 0;
    m_angularAcceleration    = 0.0;
    m_angularVelocity        = 0.0;
    m_angularImpulse         = 0.0;
    m_torque                 = 0.0;
    m_invMomentOfInertia     = std::numeric_limits<MCFloat>::max();
    m_momentOfInertia        = 0;
    m_renderLayer            = 0;
    m_renderLayerRelative    = 0;
    m_collisionLayer         = 0;
    m_index                  = -1;
    m_i0                     = 0;
    m_i1                     = 0;
    m_j0                     = 0;
    m_j1                     = 0;
    m_initialAngle           = 0;
    m_damping                = DAMPING;
    m_timerEventObjectsIndex = -1;
    m_sleeping               = false;
    m_sleepingPrevented      = false;
    m_linearSleepLimit       = 0.01f;
    m_angularSleepLimit      = 0.01f;
    m_physicsObject          = true;
    m_triggerObject          = false;
    m_stationary             = false;
    m_renderable             = true;
    m_bypassCollisions       = false;
    m_removing               = false;
    m_renderOutline          = false;
    m_isParticle             = false;
    m_parent                 = this;
    m_displacementActive     = false;
}

MCUint MCObject::getTypeIDForName(const std::string & typeName)
{
    auto i(m_typeHash.find(typeName));
    return i == m_typeHash.end() ? 0 : i->second;
}

void MCObject::addChildObject(
    MCObjectPtr object, const MCVector3dF & relativeLocation, MCFloat relativeAngle)
{
    assert(object.get() != this);
    m_children.push_back(object);
    object->setParent(*this);
    object->m_relativeLocation = relativeLocation;
    object->m_relativeAngle    = relativeAngle;
}

void MCObject::setParent(MCObject & parent)
{
    m_parent = &parent;
}

MCObject & MCObject::parent() const
{
    assert(m_parent);
    return *m_parent;
}

MCUint MCObject::registerType(const std::string & typeName)
{
    auto i(m_typeHash.find(typeName));
    if (i == m_typeHash.end())
    {
        m_typeIDCount++;
        m_typeHash[typeName] = m_typeIDCount;
        return m_typeIDCount;
    }
    else
    {
        return i->second;
    }
}

void MCObject::integrate(MCFloat step)
{
    // Integrate, if the object is not sleeping and it doesn't
    // have a parent object.
    if (!m_sleeping && m_parent == this)
    {
        integrateLinear(step);
        integrateAngular(step);
        doOutOfBoundariesEvent();

        if (m_velocity.lengthFast() < m_linearSleepLimit &&
            m_angularVelocity       < m_angularSleepLimit)
        {
            toggleSleep(true);
            resetMotion();
        }

        m_forces.setZero();
        m_linearImpulse.setZero();
        m_angularImpulse = 0.0;

        translate(location() + m_velocity);
    }
}

void MCObject::integrateLinear(MCFloat step)
{
    MCVector3dF totAcceleration(m_acceleration);
    totAcceleration += m_forces * m_invMass;
    m_velocity      += totAcceleration * step + m_linearImpulse;
    m_velocity      *= m_damping;
}

void MCObject::integrateAngular(MCFloat step)
{
    if (m_shape && m_momentOfInertia > 0.0)
    {
        MCFloat totAngularAcceleration(m_angularAcceleration);
        totAngularAcceleration += m_torque * m_invMomentOfInertia;
        m_angularVelocity      += totAngularAcceleration * step + m_angularImpulse;
        m_angularVelocity      *= m_damping;

        const MCFloat newAngle = m_angle + MCTrigonom::radToDeg(m_angularVelocity * step);
        doRotate(newAngle);
        m_angle = newAngle;
    }

    m_torque = 0.0;
}

void MCObject::doOutOfBoundariesEvent()
{
    // Use shape bbox if shape is defined.
    if (m_shape)
    {
        checkXBoundariesAndSendEvent(m_shape->bbox().x1(), m_shape->bbox().x2());
        checkYBoundariesAndSendEvent(m_shape->bbox().y1(), m_shape->bbox().y2());
    }
    else
    {
        // By default use the center point as the test point.
        checkXBoundariesAndSendEvent(m_location.i(), m_location.i());
        checkYBoundariesAndSendEvent(m_location.j(), m_location.j());
    }

    checkZBoundariesAndSendEvent();
}

void MCObject::checkXBoundariesAndSendEvent(MCFloat minX, MCFloat maxX)
{
    const MCWorld & world = MCWorld::instance();
    if (minX < world.minX())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::West);
        outOfBoundariesEvent(e);
    }
    else if (maxX > world.maxX())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::East);
        outOfBoundariesEvent(e);
    }
}

void MCObject::checkYBoundariesAndSendEvent(MCFloat minY, MCFloat maxY)
{
    const MCWorld & world = MCWorld::instance();
    if (minY < world.minY())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::South);
        outOfBoundariesEvent(e);
    }
    else if (maxY > world.maxY())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::North);
        outOfBoundariesEvent(e);
    }
}

void MCObject::checkZBoundariesAndSendEvent()
{
    const MCWorld & world = MCWorld::instance();
    if (m_location.k() < world.minZ())
    {
        m_velocity.setK(0); m_forces.setK(0);
        translate(
            MCVector3dF(m_location.i(), m_location.j(), world.minZ()));
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Bottom);
        outOfBoundariesEvent(e);
    }
    else if (m_location.k() > world.maxZ())
    {
        m_velocity.setK(0); m_forces.setK(0);
        translate(
            MCVector3dF(m_location.i(), m_location.j(), world.maxZ()));
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Top);
        outOfBoundariesEvent(e);
    }
}

void MCObject::resetMotion()
{
    // Reset linear motion
    m_forces.setZero();
    m_velocity.setZero();
    m_acceleration.setZero();
    m_linearImpulse.setZero();

    // Reset angular motion
    m_torque              = 0.0;
    m_angularAcceleration = 0.0;
    m_angularVelocity     = 0.0;
    m_angularImpulse      = 0.0;

    for (auto child: m_children)
    {
        child->resetMotion();
    }
}

MCUint MCObject::typeID() const
{
    return m_typeID;
}

MCUint MCObject::typeID(const std::string & typeName)
{
    return MCObject::getTypeIDForName(typeName);
}

bool MCObject::event(MCEvent & event)
{
    if (event.instanceTypeID() == MCCollisionEvent::typeID())
    {
        collisionEvent(static_cast<MCCollisionEvent &>(event));
        return true;
    }
    else if (event.instanceTypeID() == MCOutOfBoundariesEvent::typeID())
    {
        outOfBoundariesEvent(static_cast<MCOutOfBoundariesEvent &>(event));
        return true;
    }

    return false;
}

void MCObject::collisionEvent(MCCollisionEvent & event)
{
    event.accept();
}

void MCObject::outOfBoundariesEvent(MCOutOfBoundariesEvent & event)
{
    event.accept();
}

void MCObject::sendEvent(MCObject & object, MCEvent & event)
{
    object.event(event);
}

void MCObject::subscribeTimerEvent(MCObject & object)
{
    if (object.m_timerEventObjectsIndex == -1)
    {
        m_timerEventObjects.push_back(&object);
        object.m_timerEventObjectsIndex = m_timerEventObjects.size() - 1;
    }
}

void MCObject::unsubscribeTimerEvent(MCObject & object)
{
    if (object.m_timerEventObjectsIndex > -1)
    {
        m_timerEventObjects.back()->m_timerEventObjectsIndex =
            object.m_timerEventObjectsIndex;
        m_timerEventObjects.at(object.m_timerEventObjectsIndex) =
            m_timerEventObjects.back();
        m_timerEventObjects.pop_back();
        object.m_timerEventObjectsIndex = -1;
    }
}

void MCObject::sendTimerEvent(MCTimerEvent & event)
{
    for (MCObject * obj : m_timerEventObjects)
    {
        MCObject::sendEvent(*obj, event);
    }
}

void MCObject::addToWorld()
{
    MCWorld::instance().addObject(*this);

    for (auto child : m_children)
    {
        MCWorld::instance().addObject(*child);
    }
}

void MCObject::addToWorld(MCFloat x, MCFloat y, MCFloat z)
{
    MCWorld::instance().addObject(*this);

    for (auto child : m_children)
    {
        MCWorld::instance().addObject(*child);
    }

    translate(MCVector3dF(x, y, z));
}

void MCObject::removeFromWorld()
{
    if (MCWorld::hasInstance())
    {
        MCWorld::instance().removeObject(*this);

        for (auto child : m_children)
        {
            MCWorld::instance().removeObject(*child);
        }
    }
}

void MCObject::removeFromWorldNow()
{
    if (MCWorld::hasInstance())
    {
        MCWorld::instance().removeObjectNow(*this);

        for (auto child : m_children)
        {
            MCWorld::instance().removeObjectNow(*child);
        }
    }
}

void MCObject::render(MCCamera * p)
{
    if (m_shape)
    {
        m_shape->render(p);
    }
}

void MCObject::renderShadow(MCCamera * p)
{
    if (m_shape)
    {
        m_shape->renderShadow(p);
    }
}

void MCObject::setMass(MCFloat newMass, bool stationary)
{
    m_stationary = stationary;

    if (!stationary)
    {
        if (newMass > 0)
        {
            m_invMass = 1.0 / newMass;
        }
        else
        {
            m_invMass = std::numeric_limits<MCFloat>::max();
        }

        m_mass = newMass;

        // This is just a default guess. The shape should set the "correct" value.
        setMomentOfInertia(newMass);
    }
    else
    {
        m_invMass  = 0;
        m_mass     = std::numeric_limits<MCFloat>::max();

        m_sleeping = true;

        setMomentOfInertia(std::numeric_limits<MCFloat>::max());
    }
}

MCFloat MCObject::invMass() const
{
    return m_invMass;
}

MCFloat MCObject::mass() const
{
    return m_mass;
}

void MCObject::setMomentOfInertia(MCFloat newMomentOfInertia)
{
    if (newMomentOfInertia > 0)
    {
        m_invMomentOfInertia = 1.0 / newMomentOfInertia;
    }
    else
    {
        m_invMomentOfInertia = std::numeric_limits<MCFloat>::max();
    }

    m_momentOfInertia = newMomentOfInertia;
}

MCFloat MCObject::momentOfInertia() const
{
    return m_momentOfInertia;
}

MCFloat MCObject::invMomentOfInertia() const
{
    return m_invMomentOfInertia;
}

bool MCObject::stationary() const
{
    return m_stationary;
}

void MCObject::addImpulse(const MCVector3dF & impulse)
{
    m_linearImpulse += impulse;

    toggleSleep(false);
}

void MCObject::addImpulse(const MCVector3dF & impulse, const MCVector3dF & pos)
{
    m_linearImpulse += impulse;
    const MCFloat r = (pos - m_location).lengthFast();
    if (r > 0) {
        addAngularImpulse((-(impulse % (pos - m_location)).k()) / r);
    }
    toggleSleep(false);
}

void MCObject::addAngularImpulse(MCFloat impulse)
{
    m_angularImpulse += impulse;

    toggleSleep(false);
}

void MCObject::setIsPhysicsObject(bool flag)
{
    m_physicsObject = flag;
}

bool MCObject::isPhysicsObject() const
{
    return m_physicsObject;
}

void MCObject::setIsTriggerObject(bool flag)
{
    m_triggerObject = flag;
}

bool MCObject::isTriggerObject() const
{
    return m_triggerObject;
}

void MCObject::setBypassCollisions(bool flag)
{
    m_bypassCollisions = flag;
}

bool MCObject::bypassCollisions() const
{
    return m_bypassCollisions;
}

void MCObject::setIsRenderable(bool flag)
{
    m_renderable = flag;
}

bool MCObject::isRenderable() const
{
    return m_renderable;
}

bool MCObject::isParticle() const
{
    return m_isParticle;
}

void MCObject::setIsParticle(bool flag)
{
    m_isParticle = flag;
}

void MCObject::setVelocity(const MCVector3dF & newVelocity)
{
    m_velocity = newVelocity;

    toggleSleep(false);
}

const MCVector3dF & MCObject::velocity() const
{
    return m_velocity;
}

MCFloat MCObject::speed() const
{
    return velocity().dot(MCVector3dF(direction()));
}

void MCObject::setAngularVelocity(MCFloat newVelocity)
{
    m_angularVelocity = newVelocity;

    toggleSleep(false);
}

MCFloat MCObject::angularVelocity() const
{
    return m_angularVelocity;
}

void MCObject::setAcceleration(const MCVector3dF & newAcceleration)
{
    m_acceleration = newAcceleration;

    toggleSleep(false);
}

const MCVector3dF & MCObject::acceleration() const
{
    return m_acceleration;
}

void MCObject::translate(const MCVector3dF & newLocation)
{
    const bool wasInWorld = !removing() &&
        MCWorld::instance().objectGrid().remove(*this);

    // Calculate velocity if this object is a child object and is thus moved
    // by the parent. This way we'll automatically get linear velocity +
    // possible orbital velocity.
    // TODO: do we need to take the time step into account here?
    if (m_parent != this && !m_parent->stationary() && !m_displacementActive)
    {
        m_velocity = newLocation - m_location;
    }

    m_location = newLocation;

    if (m_shape)
    {
        m_shape->translate(newLocation);
    }

    updateChildTransforms();

    if (wasInWorld)
    {
        MCWorld::instance().objectGrid().insert(*this);
    }
}

void MCObject::displace(const MCVector3dF & displacement)
{
    m_displacementActive = true;
    translate(m_location + displacement);
    m_displacementActive = false;
}

const MCVector3dF & MCObject::location() const
{
    return m_location;
}

void MCObject::setShadowOffset(const MCVector2dF & p)
{
    assert(m_shape);
    m_shape->setShadowOffset(p);
}

void MCObject::setCenterOfRotation(MCVector2dF center)
{
    m_centerOfRotation = center - MCVector2dF(location());
}

void MCObject::rotate(MCFloat newAngle)
{
    doRotate(newAngle);
    m_angle = newAngle;

    updateChildTransforms();
}

void MCObject::rotateRelative(MCFloat newAngle)
{
    m_relativeAngle = newAngle;
}

void MCObject::doRotate(MCFloat newAngle)
{
    if (newAngle != m_angle)
    {
        if (!m_centerOfRotation.isZero())
        {
            displace(m_centerOfRotation - MCTrigonom::rotatedVector(m_centerOfRotation, newAngle - m_angle));
        }

        if (m_shape)
        {
            if (m_shape->instanceTypeID() == MCCircleShape::typeID())
            {
                m_shape->rotate(newAngle);
            }
            else
            {
                const bool wasInWorld = MCWorld::instance().objectGrid().remove(*this);

                m_shape->rotate(newAngle);

                if (wasInWorld)
                {
                    MCWorld::instance().objectGrid().insert(*this);
                }
            }
        }
    }
}

MCFloat MCObject::angle() const
{
    return m_angle;
}

MCVector2dF MCObject::direction() const
{
    return MCVector2dF(MCTrigonom::cos(angle()), MCTrigonom::sin(angle()));
}

void MCObject::setRestitution(MCFloat newRestitution)
{
    newRestitution = newRestitution < 0.0 ? 0.0 : newRestitution;
    newRestitution = newRestitution > 1.0 ? 1.0 : newRestitution;
    m_restitution = newRestitution;
}

MCFloat MCObject::restitution() const
{
    return m_restitution;
}

void MCObject::setShape(MCShapePtr shape)
{
    m_shape = shape;

    if (m_shape)
    {
        m_shape->setParent(*this);
    }
}

MCShapePtr MCObject::shape() const
{
    return m_shape;
}

void MCObject::addForce(const MCVector3dF & force)
{
    m_forces += force;

    toggleSleep(false);
}

void MCObject::addForce(const MCVector3dF & force, const MCVector3dF & pos)
{
    addTorque(-(force % (pos - m_location)).k());
    m_forces += force;

    toggleSleep(false);
}

void MCObject::addTorque(MCFloat torque)
{
    m_torque += torque;

    toggleSleep(false);
}

void MCObject::clearForces()
{
    m_forces.setZero();
    m_torque = 0.0;
}

MCBBox<MCFloat> MCObject::bbox() const
{
    if (m_shape)
    {
        return m_shape->bbox();
    }
    else
    {
        return MCBBox<MCFloat>(m_location.i(), m_location.j(), m_location.i() + 1, m_location.j() + 1);
    }
}

void MCObject::stepTime(MCFloat /* step */)
{
}

void MCObject::setRenderLayer(int layer)
{
    if (m_index != -1) // Check that the object is added to world
    {
        MCWorld::instance().renderer().removeFromLayerMap(*this);
        m_renderLayer = layer;
        MCWorld::instance().renderer().addToLayerMap(*this);
    }
    else
    {
        m_renderLayer = layer;
    }

    for (auto child : m_children)
    {
        child->setRenderLayer(m_renderLayer + child->m_renderLayerRelative);
    }
}

void MCObject::setRenderLayerRelative(int layer)
{
    m_renderLayerRelative = layer;
    if (m_parent && m_index != -1)
    {
        MCWorld::instance().renderer().removeFromLayerMap(*this);
        m_renderLayer = m_parent->renderLayer() + m_renderLayerRelative;
        MCWorld::instance().renderer().addToLayerMap(*this);
    }
}

int MCObject::renderLayer() const
{
    return m_renderLayer;
}

void MCObject::setCollisionLayer(int layer)
{
    m_collisionLayer = layer;

    for (auto child : m_children) {
        child->setCollisionLayer(layer);
    }
}

int MCObject::collisionLayer() const
{
    return m_collisionLayer;
}

void MCObject::setIndex(int newIndex)
{
    m_index = newIndex;
}

int MCObject::index() const
{
    return m_index;
}

void MCObject::cacheIndexRange(MCUint i0, MCUint i1, MCUint j0, MCUint j1)
{
    m_i0 = i0;
    m_i1 = i1;
    m_j0 = j0;
    m_j1 = j1;
}

void MCObject::restoreIndexRange(MCUint * i0, MCUint * i1, MCUint * j0, MCUint * j1)
{
    *i0 = m_i0;
    *i1 = m_i1;
    *j0 = m_j0;
    *j1 = m_j1;
}

void MCObject::setRemoving(bool flag)
{
    m_removing = flag;
}

bool MCObject::removing() const
{
    return m_removing;
}

void MCObject::addContact(MCContact & contact)
{
    m_contacts[&contact.object()].push_back(&contact);
}

const MCObject::ContactHash & MCObject::contacts() const
{
    return m_contacts;
}

void MCObject::deleteContacts()
{
    auto i(m_contacts.begin());
    for (; i != m_contacts.end(); i++)
    {
        for (MCUint j = 0; j < i->second.size(); j++)
        {
            i->second[j]->free();
        }
    }
    m_contacts.clear();
}

void MCObject::deleteContacts(MCObject & object)
{
    auto i(m_contacts.find(&object));
    if (i != m_contacts.end())
    {
        for (MCUint j = 0; j < i->second.size(); j++)
        {
            i->second[j]->free();
        }
        i->second.clear();
    }
}

void MCObject::setInitialLocation(const MCVector3dF & location)
{
    m_initialLocation = location;
}

const MCVector3dF & MCObject::initialLocation() const
{
    return m_initialLocation;
}

void MCObject::setInitialAngle(int angle)
{
    m_initialAngle = angle;
}

int MCObject::initialAngle() const
{
    return m_initialAngle;
}

void MCObject::setXYFriction(MCFloat friction)
{
    m_xyFriction = friction;
}

MCFloat MCObject::xyFriction() const
{
    return m_xyFriction;
}

void MCObject::setDamping(MCFloat value)
{
    m_damping = value;
}

bool MCObject::sleeping() const
{
    return m_sleeping;
}

void MCObject::setSleepLimits(MCFloat linearSleepLimit, MCFloat angularSleepLimit)
{
    m_linearSleepLimit  = linearSleepLimit;
    m_angularSleepLimit = angularSleepLimit;
}

void MCObject::toggleSleep(bool state)
{
    if (state && m_sleepingPrevented)
    {
        return;
    }

    m_sleeping = state;

    // Optimization: dynamically remove from the integration vector
    if (!isParticle())
    {
        if (state)
        {
            MCWorld::instance().removeObjectFromIntegration(*this);
        }
        else
        {
            MCWorld::instance().restoreObjectToIntegration(*this);
        }
    }
}

void MCObject::updateChildTransforms()
{
    for (auto child : m_children)
    {
        const float newAngle = m_angle + child->m_relativeAngle;
        child->rotate(newAngle);
        child->m_displacementActive = m_displacementActive;
        child->translate(m_location +
            MCVector3dF(MCTrigonom::rotatedVector(child->m_relativeLocation, m_angle),
                child->m_relativeLocation.k()));
    }
}

MCFloat MCObject::calculateLinearBalance(const MCVector3dF & force, const MCVector3dF & pos)
{
    MCFloat linearBalance = 1.0;
    if (shape()) {
        const MCFloat r = shape()->radius();
        if (r > 0) {
            linearBalance = 1.0 - MCMathUtil::distanceFromVector(
                MCVector2dF(pos - location()), MCVector2dF(force)) / r;
            linearBalance = linearBalance < 0 ? 0 : linearBalance;
            linearBalance = linearBalance > 1 ? 1 : linearBalance;
        }
    }
    return linearBalance;
}

void MCObject::preventSleeping(bool flag)
{
    m_sleepingPrevented = flag;
}

MCObject::~MCObject()
{
    removeFromWorldNow();
    deleteContacts();
}
