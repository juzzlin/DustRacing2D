// This file is part of Dust Racing 2D.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

static const char * BRIDGE_ID      = "bridge";
static const char * BRIDGE_RAIL_ID = "bridgeRail";

Bridge::Bridge(MCSurface & surface, MCSurface & railSurface)
: MCObject(surface, BRIDGE_ID)
, m_tag(0)
, m_rail0(MCObjectPtr(new MCObject(railSurface, BRIDGE_RAIL_ID)))
, m_rail1(MCObjectPtr(new MCObject(railSurface, BRIDGE_RAIL_ID)))
, m_railLower0(MCObjectPtr(new MCObject(railSurface, BRIDGE_RAIL_ID)))
, m_railLower1(MCObjectPtr(new MCObject(railSurface, BRIDGE_RAIL_ID)))
, m_trigger0(MCObjectPtr(new BridgeTrigger))
, m_trigger1(MCObjectPtr(new BridgeTrigger))
{
    setRenderLayer(Layers::Bridge);
    setCollisionLayer(-1);

    setIsPhysicsObject(false);
    setIsTriggerObject(true);
    setMass(0, true);

    shape()->view()->setHasShadow(false);
    shape()->view()->setBatchMode(true);

    const int railYDisplacement = 110;
    const int railZ = 8;

    addChildObject(m_rail0, MCVector3dF(0, -railYDisplacement, railZ));
    addChildObject(m_rail1, MCVector3dF(0,  railYDisplacement, railZ));

    m_rail0->setRenderLayer(Layers::BridgeRails);
    m_rail0->setCollisionLayer(Layers::BridgeRails);
    m_rail0->setMass(0, true);
    m_rail0->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

    m_rail1->setRenderLayer(Layers::BridgeRails);
    m_rail1->setCollisionLayer(Layers::BridgeRails);
    m_rail1->setMass(0, true);
    m_rail1->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

    const int railXDisplacement = 96;

    addChildObject(m_railLower0, MCVector3dF(-railXDisplacement, 0, 0));
    m_railLower0->rotateRelative(90);
    addChildObject(m_railLower1, MCVector3dF( railXDisplacement, 0, 0));
    m_railLower1->rotateRelative(90);

    m_railLower0->setRenderLayer(Layers::Walls);
    m_railLower0->setCollisionLayer(Layers::Walls);
    m_railLower0->setMass(0, true);
    m_railLower0->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));
    m_railLower0->setIsRenderable(false);

    m_railLower1->setRenderLayer(Layers::Walls);
    m_railLower1->setCollisionLayer(Layers::Walls);
    m_railLower1->setMass(0, true);
    m_railLower1->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));
    m_railLower1->setIsRenderable(false);

    const int triggerXDisplacement = 256 / 2 - 32 / 2 - 1; // The trigger boundaries must be inside the bridge tile

    addChildObject(m_trigger0, MCVector3dF(-triggerXDisplacement, 0, 0));
    addChildObject(m_trigger1, MCVector3dF( triggerXDisplacement, 0, 0));
}

void Bridge::collisionEvent(MCCollisionEvent & event)
{
    if (!event.collidingObject().stationary())
    {
        m_objectsOnBridge[&event.collidingObject()] = m_tag;
    }
}

void Bridge::stepTime(MCFloat)
{
    const int frameTolerance = 2;
    auto iter = m_objectsOnBridge.begin();
    while (iter != m_objectsOnBridge.end())
    {
        if (m_tag > iter->second + frameTolerance)
        {
            iter->first->setCollisionLayer(Layers::Ground);
            iter->first->setRenderLayer(Layers::Objects);
            iter = m_objectsOnBridge.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    m_tag++;
}
