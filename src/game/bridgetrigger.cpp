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
#include "layers.hpp"

#include "MCCollisionEvent"
#include "MCRectShape"

static const char * BRIDGE_TRIGGER_ID = "bridgeTrigger";

BridgeTrigger::BridgeTrigger()
: MCObject(BRIDGE_TRIGGER_ID)
{
    MCRectShape * shape = new MCRectShape(nullptr, 32, 224);
    setShape(MCShapePtr(shape));

    setRenderLayer(Layers::Bridge);
    setCollisionLayer(-1);

    setIsPhysicsObject(false);
    setIsTriggerObject(true);

    setMass(0, true);
}

void BridgeTrigger::collisionEvent(MCCollisionEvent & event)
{
    event.collidingObject().setCollisionLayer(Layers::BridgeRails);
    event.collidingObject().setRenderLayer(Layers::BridgeRails);
}
