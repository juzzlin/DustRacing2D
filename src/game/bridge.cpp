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

#include <MCCollisionEvent>
#include <MCRectShape>
#include <MCSurface>
#include <MCVector2d>
#include <MCPhysicsComponent>

namespace {
static const char * BRIDGE_ID      = "bridge";
static const char * BRIDGE_RAIL_ID = "bridgeRail";
static const int    RAIL_Z         = 8;
static const float  OBJECT_Z_DELTA = 5.0f;
static const float  OBJECT_Z_ZERO  = 0.0f;
static const int    WIDTH          = 256;
}

Bridge::Bridge(MCSurface & surface, MCSurface & railSurface)
: MCObject(surface, BRIDGE_ID)
, m_tag(0)
, m_rail0(MCObjectPtr(new MCObject(railSurface, BRIDGE_RAIL_ID)))
, m_rail1(MCObjectPtr(new MCObject(railSurface, BRIDGE_RAIL_ID)))
, m_trigger0(MCObjectPtr(new BridgeTrigger(*this)))
, m_trigger1(MCObjectPtr(new BridgeTrigger(*this)))
{
    setRenderLayer(static_cast<int>(Layers::Render::Objects));
    setCollisionLayer(-1);

    setIsPhysicsObject(false);
    setIsTriggerObject(true);
    physicsComponent().setMass(0, true);

    shape()->view()->setHasShadow(false);
    shape()->view()->setBatchMode(true);

    const int railYDisplacement = 110;

    addChildObject(m_rail0, MCVector3dF(0, -railYDisplacement, RAIL_Z));
    addChildObject(m_rail1, MCVector3dF(0,  railYDisplacement, RAIL_Z));

    m_rail0->setRenderLayer(static_cast<int>(Layers::Render::Objects));
    m_rail0->setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
    m_rail0->physicsComponent().setMass(0, true);
    m_rail0->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

    m_rail1->setRenderLayer(static_cast<int>(Layers::Render::Objects));
    m_rail1->setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
    m_rail1->physicsComponent().setMass(0, true);
    m_rail1->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

    const int triggerXDisplacement = WIDTH / 2;

    addChildObject(m_trigger0, MCVector3dF(-triggerXDisplacement, 0, 0));
    addChildObject(m_trigger1, MCVector3dF( triggerXDisplacement, 0, 0));
}

void Bridge::enterObject(MCObject & object)
{
    object.setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
    object.setRenderLayer(static_cast<int>(Layers::Render::Objects));

    const MCVector3dF newLocation(object.location().i(), object.location().j(), location().k() + OBJECT_Z_DELTA);
    object.translate(newLocation);

    m_objectsEntered[&object] = true;
    m_objectsOnBridge[&object] = m_tag;
}

void Bridge::collisionEvent(MCCollisionEvent & event)
{
    MCObject & object = event.collidingObject();
    if (!object.physicsComponent().isStationary())
    {
        if (m_objectsEntered.count(&object))
        {
            object.setCollisionLayer(static_cast<int>(Layers::Collision::BridgeRails));
            object.physicsComponent().preventSleeping(true);

            const MCVector3dF newLocation(object.location().i(), object.location().j(), location().k() + OBJECT_Z_DELTA);
            object.translate(newLocation);

            m_objectsOnBridge[&object] = m_tag;
        }
    }
}

void Bridge::onStepTime(MCFloat)
{
    const int frameTolerance = 2;
    auto iter = m_objectsOnBridge.begin();
    while (iter != m_objectsOnBridge.end())
    {
        if (m_tag > iter->second + frameTolerance)
        {
            MCObject & object = *iter->first;
            object.setCollisionLayer(0); // MCObject default collision layer
            object.physicsComponent().preventSleeping(false);

            const MCVector3dF newLocation(object.location().i(), object.location().j(), OBJECT_Z_ZERO);
            object.translate(newLocation);

            m_objectsEntered.erase(&object);
            iter = m_objectsOnBridge.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    m_tag++;
}
