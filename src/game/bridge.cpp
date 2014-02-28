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
#include "car.hpp"
#include "layers.hpp"
#include "renderer.hpp"

#include <MCCollisionEvent>
#include <MCRectShape>
#include <MCSurface>
#include <MCVector2d>

Bridge::Bridge(MCSurface & surface, MCSurface & rail)
: MCObject(surface, "Bridge")
, m_tag(0)
, m_rail0(MCObjectPtr(new MCObject(rail, "Rail")))
, m_rail1(MCObjectPtr(new MCObject(rail, "Rail")))
{
    const int railYDisplacement = 107;
    const int railZ = 10;
    addChildObject(m_rail0, MCVector3dF(0, -railYDisplacement, railZ));
    addChildObject(m_rail1, MCVector3dF(0,  railYDisplacement, railZ));

    setRenderLayer(Layers::Bridge);
    setCollisionLayer(-1);

    setIsPhysicsObject(false);
    setIsTriggerObject(true);
    setMass(0, true);

    shape()->view()->setHasShadow(false);
    shape()->view()->setBatchMode(true);

    m_rail0->setRenderLayer(Layers::BridgeRails);
    m_rail0->setCollisionLayer(Layers::BridgeRails);
    m_rail0->setMass(0, true);
    m_rail0->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

    m_rail1->setRenderLayer(Layers::BridgeRails);
    m_rail1->setCollisionLayer(Layers::BridgeRails);
    m_rail1->setMass(0, true);
    m_rail1->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));
}

void Bridge::collisionEvent(MCCollisionEvent & event)
{
    static MCUint carType = MCObject::typeID("Car");
    if (event.collidingObject().typeID() == carType)
    {
        Car & car = dynamic_cast<Car &>(event.collidingObject());
        const MCVector2dF bridgeOrientation(MCTrigonom::cos(angle()), MCTrigonom::sin(angle()));
        const MCFloat dot = MCVector2dF(car.velocity()).normalizedFast().dot(bridgeOrientation);
        const float directionLimit = 0.5;
        if (dot < -directionLimit || dot > directionLimit)
        {
            event.collidingObject().setCollisionLayer(Layers::BridgeRails);
            event.collidingObject().setRenderLayer(Layers::BridgeRails);
            m_carsOnBridge[&car] = m_tag;
        }
    }
}

void Bridge::stepTime(MCFloat)
{
    auto iter = m_carsOnBridge.begin();
    while (iter != m_carsOnBridge.end())
    {
        if (m_tag > iter->second + 1)
        {
            iter->first->setCollisionLayer(Layers::Ground);
            iter->first->setRenderLayer(Layers::Cars);
            iter = m_carsOnBridge.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    m_tag++;
}
