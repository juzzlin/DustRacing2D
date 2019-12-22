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
#include "../contrib/SimpleLogger/src/simple_logger.hpp"
#include "car.hpp"
#include "layers.hpp"
#include "renderer.hpp"
#include "tracktile.hpp"

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

Bridge::ObjectStatusMap Bridge::m_onBridgeStatusMap;

Bridge::ObjectStatusMap Bridge::m_underBridgeStatusMap;

class UnderBridgeTrigger : public MCObject
{
public:
    UnderBridgeTrigger(Bridge & bridge)
      : MCObject("under_bridge_trigger")
      , m_bridge(bridge)
    {
        setShape(MCShapePtr(new MCRectShape(nullptr, TrackTile::width() - TrackTile::width() / 8, TrackTile::height() + TrackTile::height() / 8)));

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
            m_bridge.objectUnderBridge(event.collidingObject(), true);
        }
    }

    //! \reimp
    inline void separationEvent(MCSeparationEvent & event) override
    {
        m_bridge.objectUnderBridge(event.separatedObject(), false);
    }

private:
    Bridge & m_bridge;
};

Bridge::Bridge()
  : MCObject("bridge")
{
    setShape(std::make_shared<MCRectShape>(nullptr, TrackTile::width(), TrackTile::height()));

    setCollisionLayer(-1);
    setIsPhysicsObject(false);
    setIsTriggerObject(true);

    physicsComponent().setMass(0, true);

    const auto bridgeRailId = "bridge_rail";
    auto && railSurface = MCAssetManager::instance().surfaceManager().surface("wallLong");
    const auto rail0 = std::make_shared<MCObject>(railSurface, bridgeRailId);
    const int railYDisplacement = 42 * static_cast<int>(TrackTile::height()) / 100;
    addChildObject(rail0, MCVector3dF(0, -railYDisplacement, RAIL_Z));
    const auto rail1 = std::make_shared<MCObject>(railSurface, bridgeRailId);
    addChildObject(rail1, MCVector3dF(0, railYDisplacement, RAIL_Z));

    const auto railLayer = static_cast<int>(Layers::Collision::BridgeRails);
    rail0->setCollisionLayer(railLayer);
    rail0->physicsComponent().setMass(0, true);
    const auto railShader = Renderer::instance().program("defaultSpecular");
    rail0->shape()->view()->setShaderProgram(railShader);
    rail1->setCollisionLayer(railLayer);
    rail1->physicsComponent().setMass(0, true);
    rail1->shape()->view()->setShaderProgram(railShader);

    const auto underBridgeTrigger = std::make_shared<UnderBridgeTrigger>(*this);
    addChildObject(underBridgeTrigger, MCVector3dF(0, 0, 0));

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
        juzzlin::L().debug() << "Object " << object->index() << " collided with bridge " << index();
        if (!Bridge::m_underBridgeStatusMap[object].count(this))
        {
            Bridge::m_onBridgeStatusMap[object].insert(this);
            juzzlin::L().debug() << "Raising object " << object->index();
            object->setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
            raiseObject(*object, true);
        }
    }
}

void Bridge::separationEvent(MCSeparationEvent & event)
{
    const auto object = &event.separatedObject();
    juzzlin::L().debug() << "Object " << object->index() << " separated with bridge " << index();
    Bridge::m_onBridgeStatusMap[object].erase(this);
    if (Bridge::m_onBridgeStatusMap[object].empty())
    {
        juzzlin::L().debug() << "Lowering object " << object->index();
        object->setCollisionLayer(0); // MCObject default collision layer
        object->physicsComponent().preventSleeping(false);
        raiseObject(*object, false);
    }
}

void Bridge::objectUnderBridge(MCObject & object, bool isUnder)
{
    if (isUnder)
    {
        if (!Bridge::m_onBridgeStatusMap[&object].count(this))
        {
            juzzlin::L().debug() << "Object " << object.index() << " under bridge " << index();
            Bridge::m_underBridgeStatusMap[&object].insert(this);
        }
    }
    else
    {
        juzzlin::L().debug() << "Object " << object.index() << " not under bridge " << index();
        Bridge::m_underBridgeStatusMap[&object].erase(this);
    }
}

void Bridge::reset()
{
    Bridge::m_onBridgeStatusMap.clear();
    Bridge::m_underBridgeStatusMap.clear();
}
