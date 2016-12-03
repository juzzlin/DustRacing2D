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

#include "mcobject.hh"

#include "mccamera.hh"
#include "mccircleshape.hh"
#include "mccollisionevent.hh"
#include "mcevent.hh"
#include "mcoutofboundariesevent.hh"
#include "mcphysicscomponent.hh"
#include "mcrectshape.hh"
#include "mcshapeview.hh"
#include "mcsurface.hh"
#include "mctimerevent.hh"
#include "mctrigonom.hh"
#include "mcsurfaceview.hh"
#include "mcworld.hh"
#include "mcworldrenderer.hh"

#include <cassert>

namespace {
const int physicsObjectBit    = 1;
const int triggerObjectBit    = 2;
const int renderableBit       = 4;
const int bypassCollisionsBit = 8;
const int hasShadowBit        = 16;
const int removingBit         = 32;
const int renderOutlineBit    = 64;
const int isParticleBit       = 128;
}

MCTypeRegistry MCObject::m_typeRegistry;
MCObject::TimerEventObjectsList MCObject::m_timerEventObjects;

MCObject::MCObject(const std::string & typeId)
    : m_typeID(MCObject::m_typeRegistry.registerType(typeId))
    , m_status(physicsObjectBit | renderableBit)
    , m_parent(this)
    , m_physicsComponent(nullptr)
{
    setPhysicsComponent(*(new MCPhysicsComponent));
}

MCObject::MCObject(MCShapePtr shape, const std::string & typeId)
    : MCObject(typeId)
{
    setShape(shape);
}

MCObject::MCObject(MCSurface & surface, const std::string & typeId, bool batchMode)
    : MCObject(typeId)
{
    // Create an MCRectShape using surface with an MCSurfaceView
    MCShapePtr rectShape(new MCRectShape(
        MCShapeViewPtr(new MCSurfaceView(surface.handle(), &surface)),
        surface.width(),
        surface.height()));

    rectShape->view()->setBatchMode(batchMode);

    setShape(rectShape);
}

MCUint MCObject::getTypeIDForName(const std::string & typeName)
{
    return MCObject::m_typeRegistry.getTypeIDForName(typeName);
}

void MCObject::addChildObject(
    MCObjectPtr object, const MCVector3dF & relativeLocation, MCFloat relativeAngle)
{
    assert(object.get() != this);
    assert(object->m_parent != this);
    m_children.push_back(object);
    object->setParent(*this);
    object->m_relativeLocation = relativeLocation;
    object->m_relativeAngle    = relativeAngle;
}

void MCObject::removeChildObject(MCObject & child)
{
    if (child.m_parent == this)
    {
        child.m_parent = &child;
        for (auto iter = m_children.begin(); iter != m_children.end(); iter++)
        {
            if ((*iter).get() == &child)
            {
                m_children.erase(iter);
                break;
            }
        }
    }
}

void MCObject::removeChildObject(MCObjectPtr child)
{
    if (child->m_parent == this)
    {
        child->m_parent = child.get();
        for (auto iter = m_children.begin(); iter != m_children.end(); iter++)
        {
            if ((*iter) == child)
            {
                m_children.erase(iter);
                break;
            }
        }
    }
}

const MCObject::Children & MCObject::children() const
{
    return m_children;
}

void MCObject::setParent(MCObject & parent)
{
    m_parent = &parent;
}

MCObject & MCObject::parent() const
{
    return *m_parent;
}

void MCObject::onStepTime(int)
{
}

void MCObject::checkBoundaries()
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
        checkXBoundariesAndSendEvent(m_location.i() - m_center.i(), m_location.i() - m_center.i());
        checkYBoundariesAndSendEvent(m_location.j() - m_center.j(), m_location.j() - m_center.j());
    }

    checkZBoundariesAndSendEvent();
}

void MCObject::checkXBoundariesAndSendEvent(MCFloat minX, MCFloat maxX)
{
    const MCWorld & world = MCWorld::instance();
    if (minX < world.minX())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::West, *this);
        outOfBoundariesEvent(e);
    }
    else if (maxX > world.maxX())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::East, *this);
        outOfBoundariesEvent(e);
    }
}

void MCObject::checkYBoundariesAndSendEvent(MCFloat minY, MCFloat maxY)
{
    const MCWorld & world = MCWorld::instance();
    if (minY < world.minY())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::South, *this);
        outOfBoundariesEvent(e);
    }
    else if (maxY > world.maxY())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::North, *this);
        outOfBoundariesEvent(e);
    }
}

void MCObject::checkZBoundariesAndSendEvent()
{
    const MCWorld & world = MCWorld::instance();
    if (m_location.k() < world.minZ())
    {
        m_physicsComponent->resetZ();
        translate(
            MCVector3dF(m_location.i(), m_location.j(), world.minZ()));
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Bottom, *this);
        outOfBoundariesEvent(e);
    }
    else if (m_location.k() > world.maxZ())
    {
        m_physicsComponent->resetZ();
        translate(
            MCVector3dF(m_location.i(), m_location.j(), world.maxZ()));
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Top, *this);
        outOfBoundariesEvent(e);
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
        object.m_timerEventObjectsIndex = static_cast<int>(m_timerEventObjects.size()) - 1;
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
            MCWorld::instance().removeObjectNow(*child);
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

void MCObject::setStatus(int bit, bool flag)
{
    if (flag)
    {
        m_status |= bit;
    }
    else
    {
        m_status &= ~bit;
    }
}

bool MCObject::testStatus(int bit) const
{
    return m_status & bit;
}

void MCObject::setIsPhysicsObject(bool flag)
{
    setStatus(physicsObjectBit, flag);
}

bool MCObject::isPhysicsObject() const
{
    return testStatus(physicsObjectBit);
}

void MCObject::setIsTriggerObject(bool flag)
{
    setStatus(triggerObjectBit, flag);
}

bool MCObject::isTriggerObject() const
{
    return testStatus(triggerObjectBit);
}

void MCObject::setBypassCollisions(bool flag)
{
    setStatus(bypassCollisionsBit, flag);
}

bool MCObject::bypassCollisions() const
{
    return testStatus(bypassCollisionsBit);
}

void MCObject::setIsRenderable(bool flag)
{
    setStatus(renderableBit, flag);
}

bool MCObject::isRenderable() const
{
    return testStatus(renderableBit);
}

void MCObject::setIsParticle(bool flag)
{
    setStatus(isParticleBit, flag);
}

bool MCObject::isParticle() const
{
    return testStatus(isParticleBit);
}

void MCObject::setRemoving(bool flag)
{
    setStatus(removingBit, flag);
}

bool MCObject::removing() const
{
    return testStatus(removingBit);
}

void MCObject::translateRelative(const MCVector3dF & newLocation)
{
    m_relativeLocation = newLocation;
}

void MCObject::translate(const MCVector3dF & newLocation)
{
    if (!m_shape)
    {
        m_location = newLocation;

        updateChildTransforms();
    }
    else
    {
        const bool wasInWorld = !removing() && MCWorld::instance().objectGrid().remove(*this);

        // Calculate velocity if this object is a child object and is thus moved
        // by the parent. This way we'll automatically get linear velocity +
        // possible orbital velocity.
        // TODO: do we need to take the time step into account here?
        if (m_parent != this &&
            m_parent->physicsComponent().isIntegrating() &&
            !m_parent->physicsComponent().isStationary())
        {
            m_physicsComponent->setVelocity(newLocation - m_location);
        }

        m_location = newLocation;

        m_shape->translate(m_location - MCVector3dF(m_center));

        updateChildTransforms();

        if (wasInWorld)
        {
            MCWorld::instance().objectGrid().insert(*this);
        }
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

const MCVector3dF & MCObject::relativeLocation() const
{
    return m_relativeLocation;
}

void MCObject::setShadowOffset(const MCVector2dF & p)
{
    assert(m_shape);
    m_shape->setShadowOffset(p);
}

void MCObject::setCenter(MCVector2dF center)
{
    m_initialCenter = center;
    updateCenter();
    rotateShape(m_angle);
}

void MCObject::rotate(MCFloat newAngle, bool updateChildTransforms_)
{
    doRotate(newAngle);
    m_angle = newAngle;

    if (updateChildTransforms_)
    {
        updateChildTransforms();
    }
}

void MCObject::rotateRelative(MCFloat newAngle)
{
    m_relativeAngle = newAngle;
}

void MCObject::doRotate(MCFloat newAngle)
{
    updateCenter();
    rotateShape(newAngle);
}

void MCObject::rotateShape(MCFloat angle)
{
    if (m_shape && std::abs(m_shape->angle() - angle) > std::numeric_limits<float>::epsilon())
    {
        if (m_shape->instanceTypeID() == MCCircleShape::typeID() && m_centerIsZero)
        {
            m_shape->rotate(angle);
        }
        else
        {
            const bool wasInWorld = MCWorld::instance().objectGrid().remove(*this);

            m_shape->rotate(angle);
            m_shape->translate(m_location - MCVector3dF(m_center));

            if (wasInWorld)
            {
                MCWorld::instance().objectGrid().insert(*this);
            }
        }
    }
}

void MCObject::updateCenter()
{
    m_center = MCMathUtil::rotatedVector(m_initialCenter, m_angle);
    m_centerIsZero = m_center.isZero();
}

MCFloat MCObject::angle() const
{
    return m_angle;
}

MCVector2dF MCObject::direction() const
{
    return MCVector2dF(MCTrigonom::cos(angle()), MCTrigonom::sin(angle()));
}

void MCObject::setShape(MCShapePtr shape)
{
    m_shape = shape;

    if (m_shape)
    {
        m_shape->setParent(*this);
        rotateShape(m_angle);
    }
}

MCShapePtr MCObject::shape() const
{
    return m_shape;
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

void MCObject::updateChildTransforms()
{
    for (auto child : m_children)
    {
        const float newAngle = m_angle + child->m_relativeAngle;
        child->rotate(newAngle);
        child->translate(m_location - MCVector3dF(m_center) +
            MCVector3dF(MCMathUtil::rotatedVector(child->m_relativeLocation, m_angle),
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

void MCObject::setPhysicsComponent(MCPhysicsComponent & physicsComponent)
{
    delete m_physicsComponent;
    m_physicsComponent = &physicsComponent;
    m_physicsComponent->setObject(*this);
}

MCPhysicsComponent & MCObject::physicsComponent()
{
    return *m_physicsComponent;
}

MCObject::~MCObject()
{
    removeFromWorldNow();
    deleteContacts();
    delete m_physicsComponent;
}
