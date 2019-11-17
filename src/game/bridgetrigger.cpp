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

#include "bridgetrigger.hpp"
#include "bridge.hpp"
#include "layers.hpp"

#include <MCCollisionEvent>
#include <MCPhysicsComponent>
#include <MCRectShape>
#include <MCSeparationEvent>

BridgeTrigger::BridgeTrigger(Bridge & bridge)
  : MCObject("bridgeTrigger")
  , m_bridge(bridge)
{
    setShape(MCShapePtr(new MCRectShape(nullptr, 16, 224)));

    setCollisionLayer(-1);
    setIsPhysicsObject(false);
    setIsTriggerObject(true);

    physicsComponent().setMass(0, true);
}

void BridgeTrigger::collisionEvent(MCCollisionEvent & event)
{
    if (!event.collidingObject().physicsComponent().isStationary())
    {
        m_bridge.enterObject(event.collidingObject());
    }
}

void BridgeTrigger::separationEvent(MCSeparationEvent & event)
{
    m_bridge.exitObject(event.separatedObject());
}
