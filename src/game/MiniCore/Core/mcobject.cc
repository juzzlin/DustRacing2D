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
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mccircleshape.hh"
#include "mcrectshape.hh"
#include "mcsurface.hh"
#include "mcworld.hh"
#include "mcevent.hh"
#include "mccollisionevent.hh"
#include "mcoutofboundariesevent.hh"
#include "mctimerevent.hh"
#include "mccamera.hh"
#include "mctrigonom.hh"
#include "mcsurfaceview.hh"

#include <cassert>

MCUint MCObject::typeIDCount = 1;
MCObject::TypeHash MCObject::typeHash;
MCObject::TimerEventObjectsList MCObject::timerEventObjects;

static const MCFloat DAMPING = 0.999;

MCObject::MCObject(const std::string & typeId)
{
    init(typeId);
}

MCObject::MCObject(MCShape * pShape, const std::string & typeId)
{
    init(typeId);

    setShape(pShape);
}

MCObject::MCObject(MCSurface * pSurface, const std::string & typeId)
{
    init(typeId);

    // Create an MCRectShape using pSurface with an MCSurfaceView
    MCRectShape * rectShape = new MCRectShape(
        new MCSurfaceView(typeId, pSurface),
        pSurface ? pSurface->width() : 0,
        pSurface ? pSurface->height() : 0);
    setShape(rectShape);
}

void MCObject::init(const std::string & typeId)
{
    m_typeID                 = registerType(typeId);
    m_time                   = 0;
    m_invMass                = std::numeric_limits<MCFloat>::max();
    m_mass                   = 0;
    m_restitution            = 0.5;
    m_xyFriction             = 0.0;
    m_angle                  = 0;
    m_angularAcceleration    = 0.0;
    m_angularVelocity        = 0.0;
    m_angularImpulse         = 0.0;
    m_maximumAngularVelocity = 4 * 3.1415;
    m_maximumVelocity        = -1;
    m_torque                 = 0.0;
    m_invMomentOfInertia     = std::numeric_limits<MCFloat>::max();
    m_momentOfInertia        = 0;
    m_layer                  = 0;
    m_index                  = -1;
    m_i0                     = 0;
    m_i1                     = 0;
    m_j0                     = 0;
    m_j1                     = 0;
    m_initialAngle           = 0;
    pShape                   = nullptr;
    damping                  = DAMPING;
    timerEventObjectsIndex   = -1;
    m_sleeping               = false;
    m_linearSleepLimit       = 0.01;
    m_angularSleepLimit      = 0.01;
    m_physicsObject          = true;
    m_stationary             = false;
    m_renderable             = true;
    m_bypassCollisions       = false;
    m_hasShadow              = true;
    m_removing               = false;
    m_renderOutline          = false;
    m_isParticle             = false;
}

void MCObject::setFlag(MCUint flag, bool enable)
{
    flags = enable ? flags | flag : flags & ~flag;
}

MCUint MCObject::getTypeIDForName(const std::string & typeName)
{
    auto i(typeHash.find(typeName));
    return i == typeHash.end() ? 0 : i->second;
}

MCUint MCObject::registerType(const std::string & typeName)
{
    auto i(typeHash.find(typeName));
    if (i == typeHash.end())
    {
        typeIDCount++;
        typeHash[typeName] = typeIDCount;
        return typeIDCount;
    }
    else
    {
        return i->second;
    }
}

void MCObject::integrate(MCFloat step)
{
    if (!m_sleeping)
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

        translate(location() + velocity());
    }
}

void MCObject::integrateLinear(MCFloat step)
{
    MCVector3dF totAcceleration(m_acceleration);
    totAcceleration += m_forces * m_invMass;
    m_velocity      += totAcceleration * step + m_linearImpulse;
    m_velocity      *= damping;

    // Note that this code doesn't take the z-component into consideration
    if (m_maximumVelocity > 0)
    {
        const MCFloat l = MCVector2dF(m_velocity).lengthFast();
        if (l > m_maximumVelocity)
        {
            m_velocity /= l;
            m_velocity *= m_maximumVelocity;
        }
    }
}

void MCObject::integrateAngular(MCFloat step)
{
    if (pShape)
    {
        if (m_momentOfInertia > 0.0)
        {
            MCFloat totAngularAcceleration(m_angularAcceleration);
            totAngularAcceleration += m_torque * m_invMomentOfInertia;
            m_angularVelocity      += totAngularAcceleration * step + m_angularImpulse;
            m_angularVelocity      *= damping;

            if (m_angularVelocity > m_maximumAngularVelocity)
            {
                m_angularVelocity = m_maximumAngularVelocity;
            }
            else if (m_angularVelocity < -m_maximumAngularVelocity)
            {
                m_angularVelocity = -m_maximumAngularVelocity;
            }

            const MCFloat newAngle = m_angle + MCTrigonom::radToDeg(m_angularVelocity * step);
            doRotate(newAngle);
            m_angle = newAngle;
        }
    }

    m_torque = 0.0;
}

void MCObject::doOutOfBoundariesEvent()
{
    // Use shape bbox if shape is defined.
    if (pShape)
    {
        checkXBoundariesAndSendEvent(pShape->bbox().x1(), pShape->bbox().x2());
        checkYBoundariesAndSendEvent(pShape->bbox().y1(), pShape->bbox().y2());
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
    if (object.timerEventObjectsIndex == -1)
    {
        timerEventObjects.push_back(&object);
        object.timerEventObjectsIndex = timerEventObjects.size() - 1;
    }
}

void MCObject::unsubscribeTimerEvent(MCObject & object)
{
    if (object.timerEventObjectsIndex > -1)
    {
        timerEventObjects.back()->timerEventObjectsIndex =
            object.timerEventObjectsIndex;
        timerEventObjects.at(object.timerEventObjectsIndex) =
            timerEventObjects.back();
        timerEventObjects.pop_back();
        object.timerEventObjectsIndex = -1;
    }
}

void MCObject::sendTimerEvent(MCTimerEvent & event)
{
    for (MCObject * obj : timerEventObjects)
    {
        MCObject::sendEvent(*obj, event);
    }
}

void MCObject::addToWorld()
{
    MCWorld::instance().addObject(*this);
}

void MCObject::addToWorld(MCFloat x, MCFloat y, MCFloat z)
{
    MCWorld::instance().addObject(*this);
    translate(MCVector3dF(x, y, z));
}

void MCObject::removeFromWorld()
{
    MCWorld::instance().removeObject(*this);
}

void MCObject::removeFromWorldNow()
{
    MCWorld::instance().removeObjectNow(*this);
}

void MCObject::render(MCCamera * p)
{
    if (pShape)
    {
        pShape->render(p);
    }
}

void MCObject::renderShadow(MCCamera * p)
{
    if (pShape)
    {
        pShape->renderShadow(p);
    }
}

void MCObject::setMass(MCFloat newMass, bool stationary_)
{
    m_stationary = stationary_;

    if (!stationary_)
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

        setMomentOfInertia(newMass * 10.0);
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

void MCObject::addLinearImpulse(const MCVector3dF & impulse)
{
    m_linearImpulse += impulse;

    toggleSleep(false);
}

void MCObject::addAngularImpulse(MCFloat impulse)
{
    m_angularImpulse += impulse;

    toggleSleep(false);
}

void MCObject::setPhysicsObject(bool flag)
{
    m_physicsObject = flag;
}

bool MCObject::physicsObject() const
{
    return m_physicsObject;
}

void MCObject::setBypassCollisions(bool flag)
{
    m_bypassCollisions = flag;
}

bool MCObject::bypassCollisions() const
{
    return m_bypassCollisions;
}

void MCObject::setRenderable(bool flag)
{
    m_renderable = flag;
}

bool MCObject::renderable() const
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

void MCObject::setRenderShapeOutline(bool flag)
{
    m_renderOutline = flag;
}

bool MCObject::renderShapeOutline() const
{
    return m_renderOutline;
}

void MCObject::setHasShadow(bool flag)
{
    m_hasShadow = flag;
}

bool MCObject::hasShadow() const
{
    return m_hasShadow;
}

void MCObject::setMaximumVelocity(MCFloat maxVelocity)
{
    m_maximumVelocity = maxVelocity;
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

void MCObject::setAngularVelocity(MCFloat newVelocity)
{
    m_angularVelocity = newVelocity;

    toggleSleep(false);
}

MCFloat MCObject::angularVelocity() const
{
    return m_angularVelocity;
}

void MCObject::setMaximumAngularVelocity(MCFloat newVelocity)
{
    m_maximumAngularVelocity = newVelocity;
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
        MCWorld::instance().objectTree().remove(*this);

    m_location = newLocation;

    if (pShape)
    {
        pShape->translate(newLocation);
    }

    if (wasInWorld)
    {
        MCWorld::instance().objectTree().insert(*this);
    }
}

void MCObject::displace(const MCVector3dF & displacement)
{
    translate(m_location + displacement);
}

const MCVector3dF & MCObject::location() const
{
    return m_location;
}

void MCObject::setShadowOffset(const MCVector2dF & p)
{
    assert(pShape);
    pShape->setShadowOffset(p);
}

MCFloat MCObject::getX() const
{
    return m_location.i();
}

MCFloat MCObject::getY() const
{
    return m_location.j();
}

MCFloat MCObject::getZ() const
{
    return m_location.k();
}

void MCObject::setCenterOfRotation(MCVector2dF center)
{
    centerOfRotation = center - MCVector2dF(location());
}

void MCObject::rotate(MCFloat newAngle)
{
    doRotate(newAngle);
    m_angle = newAngle;
}

void MCObject::doRotate(MCFloat newAngle)
{
    if (newAngle != m_angle)
    {
        if (!centerOfRotation.isZero())
        {
            MCVector2dF centerOfRotation1;
            MCTrigonom::rotatedVector(centerOfRotation, centerOfRotation1, newAngle - m_angle);
            displace(centerOfRotation - centerOfRotation1);
            centerOfRotation.setZero();
        }

        if (pShape)
        {
            if (pShape->instanceTypeID() == MCCircleShape::typeID())
            {
                pShape->rotate(newAngle);
            }
            else
            {
                const bool wasInWorld = MCWorld::instance().objectTree().remove(*this);

                pShape->rotate(newAngle);

                if (wasInWorld)
                {
                    MCWorld::instance().objectTree().insert(*this);
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

void MCObject::setShape(MCShape * newShape)
{
    delete pShape;
    pShape = newShape;

    if (pShape)
    {
        pShape->setParent(*this);
    }
}

MCShape * MCObject::shape() const
{
    return pShape;
}

void MCObject::setView(MCShapeView * newView)
{
    assert(pShape);
    pShape->setView(newView);
}

MCShapeView * MCObject::view() const
{
    return pShape ? pShape->view() : nullptr;
}

void MCObject::addForce(const MCVector3dF & force)
{
    m_forces  += force;

    toggleSleep(false);
}

void MCObject::addTorque(MCFloat torque)
{
    torque    += torque;

    toggleSleep(false);
}

void MCObject::clearForces()
{
    m_forces.setZero();
    m_torque = 0.0;
}

MCBBox<MCFloat> MCObject::bbox() const
{
    if (pShape)
    {
        return pShape->bbox();
    }
    else
    {
        return MCBBox<MCFloat>(getX(), getY(), getX() + 1, getY() + 1);
    }
}

void MCObject::stepTime()
{
    m_time++;
}

MCUint MCObject::time() const
{
    return m_time;
}

void MCObject::resetTime()
{
    m_time = 0;
}

void MCObject::setLayer(MCUint newLayer, bool updateWorldLayers)
{
    if (updateWorldLayers)
    {
        MCWorld::instance().removeFromLayerMap(*this);
        m_layer = newLayer;
        MCWorld::instance().addToLayerMap(*this);
    }
    else
    {
        m_layer = newLayer;
    }
}

MCUint MCObject::layer() const
{
    return m_layer;
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
    damping = value;
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

MCObject::~MCObject()
{
    deleteContacts();
    delete pShape;
}
