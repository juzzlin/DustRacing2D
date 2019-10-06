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
#include "bridgetrigger.hpp"
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
static const char * BRIDGE_ID      = "bridge";
static const char * BRIDGE_RAIL_ID = "bridgeRail";
static const int    RAIL_Z         = 16;
static const float  OBJECT_Z_DELTA = RAIL_Z;
static const float  OBJECT_Z_ZERO  = 0.0f;
static const int    WIDTH          = 256;
}

Bridge::Bridge()
    : MCObject(BRIDGE_ID)
{
    auto && shape = MCShapePtr(new MCRectShape(nullptr, WIDTH, WIDTH));
    setShape(shape);

    setCollisionLayer(-1);

    setIsPhysicsObject(false);
    setIsTriggerObject(true);

    physicsComponent().setMass(0, true);

    const int railYDisplacement = 110;

    auto && railSurface = MCAssetManager::instance().surfaceManager().surface("wallLong");

    auto && rail0 = MCObjectPtr(new MCObject(railSurface, BRIDGE_RAIL_ID));
    addChildObject(rail0, MCVector3dF(0, -railYDisplacement, RAIL_Z));

    auto && rail1 = MCObjectPtr(new MCObject(railSurface, BRIDGE_RAIL_ID));
    addChildObject(rail1, MCVector3dF(0,  railYDisplacement, RAIL_Z));

    rail0->setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
    rail0->physicsComponent().setMass(0, true);
    rail0->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

    rail1->setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
    rail1->physicsComponent().setMass(0, true);
    rail1->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

    const int triggerXDisplacement = WIDTH / 2;

    auto && trigger0 = MCObjectPtr(new BridgeTrigger(*this));
    addChildObject(trigger0, MCVector3dF(-triggerXDisplacement, 0, 0));

    auto && trigger1 = MCObjectPtr(new BridgeTrigger(*this));
    addChildObject(trigger1, MCVector3dF( triggerXDisplacement, 0, 0));

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
    const auto z = raise ? location().k() + OBJECT_Z_DELTA : OBJECT_Z_ZERO;

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
