// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "bridge.hpp"
#include "car.hpp"
#include "layers.hpp"
#include "renderer.hpp"

#include "../contrib/SimpleLogger/src/simple_logger.hpp"

#include <MCAssetManager>
#include <MCCollisionEvent>
#include <MCObjectFactory>
#include <MCPhysicsComponent>
#include <MCRectShape>
#include <MCSeparationEvent>
#include <MCSurface>
#include <MCVector2d>

namespace {
static const float RAIL_Z = 16;
} // namespace

class BridgeTrigger : public MCObject
{
public:
    BridgeTrigger(Bridge & bridge)
      : MCObject("bridgeTrigger")
      , m_bridge(bridge)
    {
        setShape(MCShapePtr(new MCRectShape(nullptr, 16, 224)));

        setCollisionLayer(-1);
        setIsPhysicsObject(false);
        setIsTriggerObject(true);

        physicsComponent().setMass(0, true);
    }

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event) override
    {
        if (!event.collidingObject().physicsComponent().isStationary())
        {
            m_bridge.enterObject(event.collidingObject());
        }
    }

    //! \reimp
    inline void separationEvent(MCSeparationEvent & event) override
    {
        m_bridge.exitObject(event.separatedObject());
    }

private:
    Bridge & m_bridge;
};

Bridge::Bridge()
  : MCObject("bridge")
{
    const int width = 256;
    setShape(std::make_shared<MCRectShape>(nullptr, width, width));

    setCollisionLayer(-1);
    setIsPhysicsObject(false);
    setIsTriggerObject(true);

    physicsComponent().setMass(0, true);

    const auto bridgeRailId = "bridgeRail";
    auto && railSurface = MCAssetManager::instance().surfaceManager().surface("wallLong");
    const auto rail0 = std::make_shared<MCObject>(railSurface, bridgeRailId);
    const int railYDisplacement = 110;
    addChildObject(rail0, MCVector3dF(0, -railYDisplacement, RAIL_Z));
    const auto rail1 = std::make_shared<MCObject>(railSurface, bridgeRailId);
    addChildObject(rail1, MCVector3dF(0, railYDisplacement, RAIL_Z));

    rail0->setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
    rail0->physicsComponent().setMass(0, true);
    rail0->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));
    rail1->setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
    rail1->physicsComponent().setMass(0, true);
    rail1->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

    const auto trigger0 = std::make_shared<BridgeTrigger>(*this);
    const int triggerXDisplacement = width / 2;
    addChildObject(trigger0, MCVector3dF(-triggerXDisplacement, 0, 0));
    const auto trigger1 = std::make_shared<BridgeTrigger>(*this);
    addChildObject(trigger1, MCVector3dF(triggerXDisplacement, 0, 0));

    MCMeshObjectData data("bridge");
    data.setMeshId("bridge");
    data.setSurfaceId("asphalt");
    data.setRestitution(0.9f);

    MCObjectFactory objectFactory(MCAssetManager::instance());
    auto bridgeMeshObject = objectFactory.build(data);
    bridgeMeshObject->setIsPhysicsObject(false);
    bridgeMeshObject->shape()->view()->setHasShadow(false);
    addChildObject(bridgeMeshObject, MCVector3dF(0, 0, -bridgeMeshObject->shape()->view()->object()->minZ()));
}

void Bridge::raiseObject(MCObject & object, bool raise)
{
    const auto x = object.location().i();
    const auto y = object.location().j();
    const auto z = raise ? location().k() + RAIL_Z : 0.0f;

    const MCVector3dF newLocation(x, y, z);
    object.translate(newLocation);
    const auto s = object.shape()->shadowOffset();
    object.shape()->setShadowOffset(MCVector3dF(s.i(), s.j(), z));
}

void Bridge::collisionEvent(MCCollisionEvent & event)
{
    if (!event.collidingObject().physicsComponent().isStationary())
    {
        const auto object = &event.collidingObject();
        if (m_edgeCount.count(object))
        {
            m_edgeCount[object]++;

            if (m_edgeCount[object] >= 2)
            {
                juzzlin::L().debug() << "Object " << object->index() << " is on bridge";
                object->setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
                raiseObject(*object, true);
            }
        }
        else
        {
            m_edgeCount[object] = 1;
        }
    }
}

void Bridge::separationEvent(MCSeparationEvent & event)
{
    const auto object = &event.separatedObject();
    if (m_edgeCount.count(object))
    {
        juzzlin::L().debug() << "Object " << object->index() << " is exiting bridge";
        m_edgeCount[object]--;

        if (m_edgeCount[object] <= 0)
        {
            doExitObject(*object);
        }
    }
}

void Bridge::enterObject(MCObject & object)
{
    if (m_edgeCount.count(&object))
    {
        m_edgeCount[&object]++;
    }
    else
    {
        m_edgeCount[&object] = 1;
    }

    juzzlin::L().debug() << "Object " << object.index() << " is entering trigger";
}

void Bridge::exitObject(MCObject & object)
{
    if (m_edgeCount.count(&object))
    {
        juzzlin::L().debug() << "Object " << object.index() << " is exiting trigger";
        m_edgeCount[&object]--;

        if (m_edgeCount[&object] <= 0)
        {
            doExitObject(object);
        }
    }
}

void Bridge::doExitObject(MCObject & object)
{
    m_edgeCount[&object] = 0;
    juzzlin::L().debug() << "Object " << object.index() << " is off bridge";
    object.setCollisionLayer(0); // MCObject default collision layer
    object.physicsComponent().preventSleeping(false);
    raiseObject(object, false);
}
