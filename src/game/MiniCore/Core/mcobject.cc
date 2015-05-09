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

MCUint MCObject::m_typeIDCount = 1;
MCObject::TypeHash MCObject::m_typeHash;
MCObject::TimerEventObjectsList MCObject::m_timerEventObjects;

MCObject::MCObject(const std::string & typeId)
    : m_physicsComponent(nullptr)
{
    init(typeId);
}

MCObject::MCObject(MCShapePtr shape, const std::string & typeId)
    : m_physicsComponent(nullptr)
{
    init(typeId);

    setShape(shape);
}

MCObject::MCObject(MCSurface & surface, const std::string & typeId, bool batchMode)
    : m_physicsComponent(nullptr)
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
    setPhysicsComponent(*(new MCPhysicsComponent));

    m_typeID                 = registerType(typeId);
    m_angle                  = 0;
    m_relativeAngle          = 0;
    m_renderLayer            = 0;
    m_renderLayerRelative    = 0;
    m_collisionLayer         = 0;
    m_index                  = -1;
    m_i0                     = 0;
    m_i1                     = 0;
    m_j0                     = 0;
    m_j1                     = 0;
    m_initialAngle           = 0;
    m_timerEventObjectsIndex = -1;
    m_physicsObject          = true;
    m_triggerObject          = false;
    m_renderable             = true;
    m_bypassCollisions       = false;
    m_removing               = false;
    m_renderOutline          = false;
    m_isParticle             = false;
    m_parent                 = this;
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

void MCObject::stepTime(MCFloat step)
{
    m_physicsComponent->stepTime(step);
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
        checkXBoundariesAndSendEvent(m_location.i(), m_location.i());
        checkYBoundariesAndSendEvent(m_location.j(), m_location.j());
    }

    checkZBoundariesAndSendEvent();
}

void MCObject::checkXBoundariesAndSendEvent(MCFloat minX, MCFloat maxX)
{
    const MCWorld & world = MCWorld::instance();
    if (minX < world.minx())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::West);
        outOfBoundariesEvent(e);
    }
    else if (maxX > world.maxx())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::East);
        outOfBoundariesEvent(e);
    }
}

void MCObject::checkYBoundariesAndSendEvent(MCFloat minY, MCFloat maxY)
{
    const MCWorld & world = MCWorld::instance();
    if (minY < world.miny())
    {
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::South);
        outOfBoundariesEvent(e);
    }
    else if (maxY > world.maxy())
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
        m_physicsComponent->resetZ();
        translate(
            MCVector3dF(m_location.i(), m_location.j(), world.minZ()));
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Bottom);
        outOfBoundariesEvent(e);
    }
    else if (m_location.k() > world.maxZ())
    {
        m_physicsComponent->resetZ();
        translate(
            MCVector3dF(m_location.i(), m_location.j(), world.maxZ()));
        MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Top);
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

void MCObject::translate(const MCVector3dF & newLocation)
{
    const bool wasInWorld = !removing() &&
        MCWorld::instance().objectGrid().remove(*this);

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
    translate(m_location + displacement);
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

void MCObject::onStepTime(MCFloat /* step */)
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

void MCObject::updateChildTransforms()
{
    for (auto child : m_children)
    {
        const float newAngle = m_angle + child->m_relativeAngle;
        child->rotate(newAngle);
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
