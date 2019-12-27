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
#include "mcseparationevent.hh"
#include "mcshapeview.hh"
#include "mcsurface.hh"
#include "mcsurfaceview.hh"
#include "mctimerevent.hh"
#include "mctrigonom.hh"
#include "mcworld.hh"
#include "mcworldrenderer.hh"

#include <bitset>
#include <cassert>
#include <map>
#include <unordered_map>
#include <vector>

class MCObject::Impl
{
public:
    enum class StatusBit
    {
        BypassCollisions = 1,
        Particle = 2,
        PhysicsObject = 3,
        Removing = 4,
        Renderable = 5,
        TriggerObject = 6
    };

    Impl(MCObject & parent, const std::string & typeName)
      : m_typeId(Impl::m_typeRegistry.registerType(typeName))
      , m_typeName(typeName)
      , m_this(parent)
      , m_parent(&parent)
      , m_physicsComponent(std::make_unique<MCPhysicsComponent>())
    {
        setStatus(StatusBit::Renderable, true);
        setStatus(StatusBit::PhysicsObject, true);
        m_physicsComponent->setObject(m_this);
    }

    Impl(MCObject & parent, MCShapePtr shape, const std::string & typeName)
      : Impl(parent, typeName)
    {
        setShape(shape);
    }

    Impl(MCObject & parent, MCSurfacePtr surface, const std::string & typeName)
      : Impl(parent, typeName)
    {
        // Create an MCRectShape using surface with an MCSurfaceView
        setShape(
          std::make_shared<MCRectShape>(
            std::make_shared<MCSurfaceView>(surface->handle(), surface), surface->width(), surface->height()));
    }

    void addToWorld()
    {
        MCWorld::instance().addObject(m_this);

        for (auto child : m_children)
        {
            MCWorld::instance().addObject(*child);
        }
    }

    void addToWorld(float x, float y, float z)
    {
        MCWorld::instance().addObject(m_this);

        for (auto child : m_children)
        {
            MCWorld::instance().addObject(*child);
        }

        translate(MCVector3dF(x, y, z));
    }

    void removeFromWorld()
    {
        if (MCWorld::hasInstance())
        {
            MCWorld::instance().removeObject(m_this);

            for (auto child : m_children)
            {
                MCWorld::instance().removeObjectNow(*child);
            }
        }
    }

    void removeFromWorldNow()
    {
        if (MCWorld::hasInstance())
        {
            MCWorld::instance().removeObjectNow(m_this);

            for (auto child : m_children)
            {
                MCWorld::instance().removeObjectNow(*child);
            }
        }
    }

    void checkBoundaries()
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

    static size_t getTypeIdForName(const std::string & typeName)
    {
        return Impl::m_typeRegistry.getTypeIdForName(typeName);
    }

    const std::string & typeName() const
    {
        return m_typeName;
    }

    void addChildObject(MCObjectPtr object, const MCVector3dF & relativeLocation, float relativeAngle)
    {
        assert(object.get() != &m_this);
        assert(object->m_impl->m_parent != m_parent);
        m_children.push_back(object);
        object->m_impl->setParent(m_this);
        object->m_impl->m_relativeLocation = relativeLocation;
        object->m_impl->m_relativeAngle = relativeAngle;
    }

    const MCObject::Children & children() const
    {
        return m_children;
    }

    void removeChildObject(MCObject & child)
    {
        if (child.m_impl->m_parent == &m_this)
        {
            child.m_impl->m_parent = &child;
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

    void removeChildObject(MCObjectPtr child)
    {
        if (child->m_impl->m_parent == &m_this)
        {
            child->m_impl->m_parent = child.get();
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

    void addContact(MCContact & contact)
    {
        m_contacts[&contact.object()].push_back(&contact);
    }

    const MCObject::ContactHash & contacts() const
    {
        return m_contacts;
    }

    void setCollisionLayer(int layer)
    {
        m_collisionLayer = layer;

        for (auto child : m_children)
        {
            child->setCollisionLayer(layer);
        }
    }

    int collisionLayer() const
    {
        return m_collisionLayer;
    }

    void setIndex(int newIndex)
    {
        m_index = newIndex;
    }

    int index() const
    {
        return m_index;
    }

    void setPhysicsComponent(std::unique_ptr<MCPhysicsComponent> physicsComponent)
    {
        m_physicsComponent = std::move(physicsComponent);
        m_physicsComponent->setObject(m_this);
    }

    MCPhysicsComponent & physicsComponent()
    {
        assert(m_physicsComponent);
        return *m_physicsComponent;
    }

    static void subscribeTimerEvent(MCObject & object)
    {
        if (object.m_impl->m_timerEventObjectsIndex == -1)
        {
            m_timerEventObjects.push_back(&object);
            object.m_impl->m_timerEventObjectsIndex = static_cast<int>(m_timerEventObjects.size()) - 1;
        }
    }

    static void unsubscribeTimerEvent(MCObject & object)
    {
        if (object.m_impl->m_timerEventObjectsIndex > -1)
        {
            m_timerEventObjects.back()->m_impl->m_timerEventObjectsIndex =
              object.m_impl->m_timerEventObjectsIndex;
            m_timerEventObjects.at(static_cast<size_t>(object.m_impl->m_timerEventObjectsIndex)) = m_timerEventObjects.back();
            m_timerEventObjects.pop_back();
            object.m_impl->m_timerEventObjectsIndex = -1;
        }
    }

    void render(MCCamera * p)
    {
        if (m_shape)
        {
            m_shape->render(p);
        }
    }

    void renderShadow(MCCamera * p)
    {
        if (m_shape)
        {
            m_shape->renderShadow(p);
        }
    }

    void setShape(MCShapePtr shape)
    {
        m_shape = shape;

        if (m_shape)
        {
            m_shape->setParent(m_this);
            rotateShape(m_angle);
        }
    }

    MCShapePtr shape() const
    {
        return m_shape;
    }

    const MCVector3dF & location() const
    {
        return m_location;
    }

    const MCVector3dF & relativeLocation() const
    {
        return m_relativeLocation;
    }

    void setInitialLocation(const MCVector3dF & location)
    {
        m_initialLocation = location;
    }

    const MCVector3dF & initialLocation() const
    {
        return m_initialLocation;
    }

    void setInitialAngle(int angle)
    {
        m_initialAngle = angle;
    }

    int initialAngle() const
    {
        return m_initialAngle;
    }

    void translateRelative(const MCVector3dF & newLocation)
    {
        m_relativeLocation = newLocation;
    }

    void translate(const MCVector3dF & newLocation)
    {
        if (!m_shape)
        {
            m_location = newLocation;

            updateChildTransforms();
        }
        else
        {
            const bool wasInWorld = !removing() && MCWorld::instance().objectGrid().remove(m_this);

            // Calculate velocity if this object is a child object and is thus moved
            // by the parent. This way we'll automatically get linear velocity +
            // possible orbital velocity.
            // TODO: do we need to take the time step into account here?
            if (m_parent != &m_this && m_parent->physicsComponent().isIntegrating() && !m_parent->physicsComponent().isStationary())
            {
                m_physicsComponent->setVelocity(newLocation - m_location);
            }

            m_location = newLocation;

            m_shape->translate(m_location - MCVector3dF(m_center));

            updateChildTransforms();

            if (wasInWorld)
            {
                MCWorld::instance().objectGrid().insert(m_this);
            }
        }
    }

    void deleteContacts()
    {
        for (auto && contact : m_contacts)
        {
            for (size_t j = 0; j < contact.second.size(); j++)
            {
                contact.second[j]->free();
            }
        }
        m_contacts.clear();
    }

    void deleteContacts(MCObject & object)
    {
        const auto iter(m_contacts.find(&object));
        if (iter != m_contacts.end())
        {
            for (size_t j = 0; j < iter->second.size(); j++)
            {
                iter->second[j]->free();
            }
            iter->second.clear();
        }
    }

    void displace(const MCVector3dF & displacement)
    {
        translate(m_location + displacement);
    }

    float angle() const
    {
        return m_angle;
    }

    void rotate(float newAngle, bool updateChildTransforms)
    {
        doRotate(newAngle);
        m_angle = newAngle;

        if (updateChildTransforms)
        {
            this->updateChildTransforms();
        }
    }

    void rotateRelative(float newAngle)
    {
        m_relativeAngle = newAngle;
    }

    void setShadowOffset(const MCVector2dF & p)
    {
        assert(m_shape);
        m_shape->setShadowOffset(p);
    }

    void setCenter(MCVector2dF center)
    {
        m_initialCenter = center;
        updateCenter();
        rotateShape(m_angle);
    }

    static void sendTimerEvent(MCTimerEvent & event)
    {
        for (auto && obj : m_timerEventObjects)
        {
            MCObject::sendEvent(*obj, event);
        }
    }

    size_t typeId() const
    {
        return m_typeId;
    }

    size_t typeId(const std::string & typeName)
    {
        return MCObject::getTypeIdForName(typeName);
    }

    void setIsPhysicsObject(bool flag)
    {
        setStatus(StatusBit::PhysicsObject, flag);
    }

    bool isPhysicsObject() const
    {
        return testStatus(StatusBit::PhysicsObject);
    }

    void setIsTriggerObject(bool flag)
    {
        setStatus(StatusBit::TriggerObject, flag);
    }

    bool isTriggerObject() const
    {
        return testStatus(StatusBit::TriggerObject);
    }

    void setBypassCollisions(bool flag)
    {
        setStatus(StatusBit::BypassCollisions, flag);
    }

    bool bypassCollisions() const
    {
        return testStatus(StatusBit::BypassCollisions);
    }

    void setIsRenderable(bool flag)
    {
        setStatus(StatusBit::Renderable, flag);
    }

    bool isRenderable() const
    {
        return testStatus(StatusBit::Renderable);
    }

    void setIsParticle(bool flag)
    {
        setStatus(StatusBit::Particle, flag);
    }

    bool isParticle() const
    {
        return testStatus(StatusBit::Particle);
    }

    void cacheIndexRange(size_t i0, size_t i1, size_t j0, size_t j1)
    {
        m_i0 = i0;
        m_i1 = i1;
        m_j0 = j0;
        m_j1 = j1;
    }

    void restoreIndexRange(size_t * i0, size_t * i1, size_t * j0, size_t * j1)
    {
        *i0 = m_i0;
        *i1 = m_i1;
        *j0 = m_j0;
        *j1 = m_j1;
    }

    void setRemoving(bool flag)
    {
        setStatus(StatusBit::Removing, flag);
    }

    bool removing() const
    {
        return testStatus(StatusBit::Removing);
    }

    MCObject & parent() const
    {
        return *m_parent;
    }

    ~Impl()
    {
        removeFromWorldNow();
        deleteContacts();
    }

private:
    float calculateLinearBalance(const MCVector3dF & force, const MCVector3dF & pos)
    {
        float linearBalance = 1.0f;
        if (shape())
        {
            const float r = shape()->radius();
            if (r > 0)
            {
                linearBalance = 1.0f - MCMathUtil::distanceFromVector(MCVector2dF(pos - location()), MCVector2dF(force)) / r;
                linearBalance = linearBalance < 0 ? 0 : linearBalance;
                linearBalance = linearBalance > 1 ? 1 : linearBalance;
            }
        }
        return linearBalance;
    }

    void checkXBoundariesAndSendEvent(float minX, float maxX)
    {
        const MCWorld & world = MCWorld::instance();
        if (minX < world.minX())
        {
            MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::West, m_this);
            m_this.outOfBoundariesEvent(e);
        }
        else if (maxX > world.maxX())
        {
            MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::East, m_this);
            m_this.outOfBoundariesEvent(e);
        }
    }

    void checkYBoundariesAndSendEvent(float minY, float maxY)
    {
        const MCWorld & world = MCWorld::instance();
        if (minY < world.minY())
        {
            MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::South, m_this);
            m_this.outOfBoundariesEvent(e);
        }
        else if (maxY > world.maxY())
        {
            MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::North, m_this);
            m_this.outOfBoundariesEvent(e);
        }
    }

    void checkZBoundariesAndSendEvent()
    {
        const MCWorld & world = MCWorld::instance();
        if (m_location.k() < world.minZ())
        {
            m_physicsComponent->resetZ();
            translate(
              MCVector3dF(m_location.i(), m_location.j(), world.minZ()));
            MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Bottom, m_this);
            m_this.outOfBoundariesEvent(e);
        }
        else if (m_location.k() > world.maxZ())
        {
            m_physicsComponent->resetZ();
            translate(
              MCVector3dF(m_location.i(), m_location.j(), world.maxZ()));
            MCOutOfBoundariesEvent e(MCOutOfBoundariesEvent::Top, m_this);
            m_this.outOfBoundariesEvent(e);
        }
    }

    void doRotate(float newAngle)
    {
        updateCenter();
        rotateShape(newAngle);
    }

    void rotateShape(float angle)
    {
        if (m_shape && std::fabs(m_shape->angle() - angle) > std::numeric_limits<float>::epsilon())
        {
            if (m_shape->type() == MCShape::Type::Circle && m_centerIsZero)
            {
                m_shape->rotate(angle);
            }
            else
            {
                const bool wasInWorld = MCWorld::instance().objectGrid().remove(m_this);

                m_shape->rotate(angle);
                m_shape->translate(m_location - MCVector3dF(m_center));

                if (wasInWorld)
                {
                    MCWorld::instance().objectGrid().insert(m_this);
                }
            }
        }
    }

    void setParent(MCObject & parent)
    {
        m_parent = &parent;
    }

    void updateChildTransforms()
    {
        for (auto child : m_children)
        {
            const float newAngle = m_angle + child->m_impl->m_relativeAngle;
            child->rotate(newAngle);
            child->translate(m_location - MCVector3dF(m_center) + MCVector3dF(MCMathUtil::rotatedVector(child->m_impl->m_relativeLocation, m_angle), child->m_impl->m_relativeLocation.k()));
        }
    }

    void updateCenter()
    {
        m_center = MCMathUtil::rotatedVector(m_initialCenter, m_angle);
        m_centerIsZero = m_center.isZero();
    }

    void setStatus(StatusBit bit, bool flag)
    {
        m_status.set(static_cast<size_t>(bit), flag);
    }

    bool testStatus(StatusBit bit) const
    {
        return m_status.test(static_cast<size_t>(bit));
    }

    static MCTypeRegistry m_typeRegistry;

    size_t m_typeId;

    std::string m_typeName;

    float m_angle = 0; // Degrees

    float m_relativeAngle = 0; // Degrees

    int m_collisionLayer = 0;

    int m_index = -1;

    size_t m_i0 = 0;

    size_t m_i1 = 0;

    size_t m_j0 = 0;

    size_t m_j1 = 0;

    MCVector3dF m_initialLocation;

    int m_initialAngle = 0;

    MCVector3dF m_location;

    MCVector3dF m_relativeLocation;

    MCVector2dF m_initialCenter;

    MCVector2dF m_center;

    bool m_centerIsZero = false;

    MCShapePtr m_shape;

    typedef std::vector<MCObject *> TimerEventObjectsList;

    static TimerEventObjectsList m_timerEventObjects;

    MCObject::ContactHash m_contacts;

    int m_timerEventObjectsIndex = -1;

    std::bitset<8> m_status;

    Children m_children;

    MCObject & m_this;

    MCObject * m_parent = nullptr;

    std::unique_ptr<MCPhysicsComponent> m_physicsComponent;
};

MCTypeRegistry MCObject::Impl::m_typeRegistry;
MCObject::Impl::TimerEventObjectsList MCObject::Impl::m_timerEventObjects;

MCObject::MCObject(const std::string & typeName)
  : m_impl(std::make_unique<Impl>(*this, typeName))
{
}

MCObject::MCObject(MCShapePtr shape, const std::string & typeName)
  : m_impl(std::make_unique<Impl>(*this, shape, typeName))
{
}

MCObject::MCObject(MCSurfacePtr surface, const std::string & typeName)
  : m_impl(std::make_unique<Impl>(*this, surface, typeName))
{
}

size_t MCObject::getTypeIdForName(const std::string & typeName)
{
    return Impl::getTypeIdForName(typeName);
}

const std::string & MCObject::typeName() const
{
    return m_impl->typeName();
}

void MCObject::addChildObject(MCObjectPtr object, const MCVector3dF & relativeLocation, float relativeAngle)
{
    m_impl->addChildObject(object, relativeLocation, relativeAngle);
}

void MCObject::removeChildObject(MCObject & child)
{
    m_impl->removeChildObject(child);
}

void MCObject::removeChildObject(MCObjectPtr child)
{
    m_impl->removeChildObject(child);
}

const MCObject::Children & MCObject::children() const
{
    return m_impl->children();
}

MCObject & MCObject::parent() const
{
    return m_impl->parent();
}

void MCObject::onStepTime(int)
{
}

void MCObject::checkBoundaries()
{
    m_impl->checkBoundaries();
}


size_t MCObject::typeId() const
{
    return m_impl->typeId();
}

size_t MCObject::typeId(const std::string & typeName)
{
    return Impl::getTypeIdForName(typeName);
}

bool MCObject::event(MCEvent & event)
{
    if (event.instanceTypeId() == MCCollisionEvent::typeId())
    {
        collisionEvent(static_cast<MCCollisionEvent &>(event));
        return true;
    }
    else if (event.instanceTypeId() == MCSeparationEvent::typeId())
    {
        separationEvent(static_cast<MCSeparationEvent &>(event));
        return true;
    }
    else if (event.instanceTypeId() == MCOutOfBoundariesEvent::typeId())
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

void MCObject::separationEvent(MCSeparationEvent & event)
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
    Impl::subscribeTimerEvent(object);
}

void MCObject::unsubscribeTimerEvent(MCObject & object)
{
    Impl::unsubscribeTimerEvent(object);
}

void MCObject::sendTimerEvent(MCTimerEvent & event)
{
    Impl::sendTimerEvent(event);
}

void MCObject::addToWorld()
{
    m_impl->addToWorld();
}

void MCObject::addToWorld(float x, float y, float z)
{
    m_impl->addToWorld(x, y, z);
}

void MCObject::removeFromWorld()
{
    m_impl->removeFromWorld();
}

void MCObject::removeFromWorldNow()
{
    m_impl->removeFromWorldNow();
}

void MCObject::render(MCCamera * p)
{
    m_impl->render(p);
}

void MCObject::renderShadow(MCCamera * p)
{
    m_impl->renderShadow(p);
}

void MCObject::setIsPhysicsObject(bool flag)
{
    m_impl->setIsPhysicsObject(flag);
}

bool MCObject::isPhysicsObject() const
{
    return m_impl->isPhysicsObject();
}

void MCObject::setIsTriggerObject(bool flag)
{
    m_impl->setIsTriggerObject(flag);
}

bool MCObject::isTriggerObject() const
{
    return m_impl->isTriggerObject();
}

void MCObject::setBypassCollisions(bool flag)
{
    m_impl->setBypassCollisions(flag);
}

bool MCObject::bypassCollisions() const
{
    return m_impl->bypassCollisions();
}

void MCObject::setIsRenderable(bool flag)
{
    m_impl->setIsRenderable(flag);
}

bool MCObject::isRenderable() const
{
    return m_impl->isRenderable();
}

void MCObject::setIsParticle(bool flag)
{
    m_impl->setIsParticle(flag);
}

bool MCObject::isParticle() const
{
    return m_impl->isParticle();
}

void MCObject::translateRelative(const MCVector3dF & newLocation)
{
    m_impl->translateRelative(newLocation);
}

void MCObject::translate(const MCVector3dF & newLocation)
{
    m_impl->translate(newLocation);
}

void MCObject::displace(const MCVector3dF & displacement)
{
    m_impl->displace(displacement);
}

const MCVector3dF & MCObject::location() const
{
    return m_impl->location();
}

const MCVector3dF & MCObject::relativeLocation() const
{
    return m_impl->relativeLocation();
}

void MCObject::setShadowOffset(const MCVector2dF & p)
{
    m_impl->setShadowOffset(p);
}

void MCObject::setCenter(MCVector2dF center)
{
    m_impl->setCenter(center);
}

void MCObject::rotate(float newAngle, bool updateChildTransforms)
{
    m_impl->rotate(newAngle, updateChildTransforms);
}

void MCObject::rotateRelative(float newAngle)
{
    m_impl->rotateRelative(newAngle);
}

float MCObject::angle() const
{
    return m_impl->angle();
}

MCVector2dF MCObject::direction() const
{
    return MCVector2dF(MCTrigonom::cos(angle()), MCTrigonom::sin(angle()));
}

void MCObject::setShape(MCShapePtr shape)
{
    m_impl->setShape(shape);
}

MCShapePtr MCObject::shape() const
{
    return m_impl->shape();
}

void MCObject::setCollisionLayer(int layer)
{
    m_impl->setCollisionLayer(layer);
}

int MCObject::collisionLayer() const
{
    return m_impl->collisionLayer();
}

void MCObject::cacheIndexRange(size_t i0, size_t i1, size_t j0, size_t j1)
{
    m_impl->cacheIndexRange(i0, i1, j0, j1);
}

void MCObject::restoreIndexRange(size_t * i0, size_t * i1, size_t * j0, size_t * j1)
{
    m_impl->restoreIndexRange(i0, i1, j0, j1);
}

void MCObject::setIndex(int index)
{
    m_impl->setIndex(index);
}

void MCObject::setRemoving(bool flag)
{
    m_impl->setRemoving(flag);
}

bool MCObject::removing() const
{
    return m_impl->removing();
}

int MCObject::index() const
{
    return m_impl->index();
}

void MCObject::addContact(MCContact & contact)
{
    m_impl->addContact(contact);
}

const MCObject::ContactHash & MCObject::contacts() const
{
    return m_impl->contacts();
}

void MCObject::deleteContacts()
{
    m_impl->deleteContacts();
}

void MCObject::deleteContacts(MCObject & object)
{
    m_impl->deleteContacts(object);
}

void MCObject::setInitialLocation(const MCVector3dF & location)
{
    m_impl->setInitialLocation(location);
}

const MCVector3dF & MCObject::initialLocation() const
{
    return m_impl->initialLocation();
}

void MCObject::setInitialAngle(int angle)
{
    m_impl->setInitialAngle(angle);
}

int MCObject::initialAngle() const
{
    return m_impl->initialAngle();
}

void MCObject::setPhysicsComponent(std::unique_ptr<MCPhysicsComponent> physicsComponent)
{
    m_impl->setPhysicsComponent(std::move(physicsComponent));
}

MCPhysicsComponent & MCObject::physicsComponent()
{
    return m_impl->physicsComponent();
}

MCObject::~MCObject() = default;
