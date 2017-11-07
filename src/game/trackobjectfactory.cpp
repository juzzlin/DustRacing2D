// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "trackobjectfactory.hpp"

#include "layers.hpp"
#include "pit.hpp"
#include "renderer.hpp"
#include "trackobject.hpp"
#include "tree.hpp"

#include <MCAssetManager>
#include <MCLogger>
#include <MCObject>
#include <MCObjectFactory>
#include <MCRandom>
#include <MCPhysicsComponent>
#include <MCShape>
#include <MCShapeView>
#include <MCSurface>

TrackObjectFactory::TrackObjectFactory(MCObjectFactory & objectFactory)
: m_objectFactory(objectFactory)
{
}

TrackObject * TrackObjectFactory::build(
    QString category, QString role, MCVector2dF location, int angle)
{
    MCObjectPtr object;

    if (role == "brake" || role == "left" || role == "right")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setMass(1000);
        data.setSurfaceId(role.toStdString());

        object = m_objectFactory.build(data);
        object->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

        // Wrap the MCObject in a TrackObject
        return new TrackObject(category, role, object);
    }
    else if (role == "bushArea")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setIsStationary(true);
        data.setSurfaceId(role.toStdString());

        object = m_objectFactory.build(data);
        object->setIsPhysicsObject(false);
    }
    else if (role == "crate")
    {
        MCMeshObjectData data(role.toStdString());
        data.setInitialLocation(MCVector3dF(location.i(), location.j(), 12));
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setMass(1000);
        data.setMeshId(role.toStdString());
        data.setSurfaceId(role.toStdString());
        data.setRestitution(0.9);

        object = m_objectFactory.build(data);
    }
    else if (role == "dustRacing2DBanner")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);

        data.setMass(15000);
        data.setSurfaceId(role.toStdString());

        object = m_objectFactory.build(data);
        object->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

        // Wrap the MCObject in a TrackObject
        return new TrackObject(category, role, object);
    }
    else if (role == "grandstand")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setMass(20000);
        data.setSurfaceId(role.toStdString());

        object = m_objectFactory.build(data);
    }
    else if (role == "plant")
    {
        const MCFloat plantBodyRadius = 4;

        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setMass(250);
        data.setSurfaceId(role.toStdString());
        data.setRestitution(0.25);
        data.setShapeWidth(plantBodyRadius);
        data.setShapeHeight(plantBodyRadius);

        object = m_objectFactory.build(data);
    }
    else if (role == "rock")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setMass(2500);
        data.setSurfaceId(role.toStdString());
        data.setRestitution(0.9);

        object = m_objectFactory.build(data);
    }
    else if (
        role == "grid"          ||
        role == "sandAreaCurve" ||
        role == "sandAreaBig")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setIsStationary(true);
        data.setSurfaceId(role.toStdString());

        object = m_objectFactory.build(data);
        object->setIsPhysicsObject(false);
        object->shape()->view()->setHasShadow(false);
    }
    else if (role == "pit")
    {
        object = MCObjectPtr(new Pit(MCAssetManager::surfaceManager().surface("pit")));
        object->setInitialLocation(location);
        object->setInitialAngle(angle);
    }
    else if (role == "tire")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setDefaultCircleShape(true);
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setMass(500); // Exaggerate the mass on purpose
        data.setSurfaceId(role.toStdString());
        data.setXYFriction(0.25);

        object = m_objectFactory.build(data);
        object->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

        // Wrap the MCObject in a TrackObject
        return new TrackObject(category, role, object);
    }
    else if (role == "tree")
    {
        int height = 200 + 200 * MCRandom::getValue();
        object = MCObjectPtr(new Tree(MCAssetManager::surfaceManager().surface("tree"),
            1.0f + 0.50f * MCRandom::getValue(),
            0.2f + 0.33f * MCRandom::getValue(),
            height,
            height / 10));
        object->setInitialLocation(location);
        object->setInitialAngle(angle);
    }
    else if (role == "wall" || role == "wallLong")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        const int wallMass = 10000;
        data.setMass(role == "wall" ? wallMass : wallMass * 4);
        data.setRestitution(0.9);
        data.setInitialLocation(MCVector3dF(location.i(), location.j(), 8));

        object = m_objectFactory.build(data);
        object->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));

        // Wrap the MCObject in a TrackObject
        return new TrackObject(category, role, object);
    }

    if (!object)
    {
        MCLogger().warning() << "Unknown or deprecated object '" << role.toStdString() << "'";
        return nullptr;
    }
    else
    {
        // Wrap the MCObject in a TrackObject
        return new TrackObject(category, role, object);
    }
}
