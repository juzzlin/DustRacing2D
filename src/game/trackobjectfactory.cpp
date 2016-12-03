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
#include "treeview.hpp"

#include <MCAssetManager>
#include <MCLogger>
#include <MCObject>
#include <MCObjectFactory>
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
        data.setBatchMode(true);
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
        data.setBatchMode(true);
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
        data.setBatchMode(true);
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
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setIsStationary(true);
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
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setIsStationary(true);
        data.setSurfaceId(role.toStdString());

        object = m_objectFactory.build(data);
    }
    else if (role == "plant")
    {
        const MCFloat plantBodyRadius = 4;

        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setIsStationary(true);
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
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setMass(5000);
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
        data.setBatchMode(true);
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
        object->physicsComponent().setMass(1, true); // Stationary
        object->setIsPhysicsObject(false);
        object->setIsTriggerObject(true);
        object->shape()->view()->setHasShadow(false);
        object->shape()->view()->setBatchMode(true);
    }
    else if (role == "tire")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setDefaultCircleShape(true);
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
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
        const MCFloat treeViewRadius = 48;
        const MCFloat treeBodyRadius = 8;

        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setIsStationary(true);
        data.setRestitution(0.25);
        data.setShapeWidth(treeBodyRadius);
        data.setShapeHeight(treeBodyRadius);

        // Create a custom view.
        MCShapeViewPtr view(new TreeView(
            MCAssetManager::surfaceManager().surface("tree"), treeViewRadius, 2, 120, 5));
        MCObjectPtr object = m_objectFactory.build(data, view);

        // Wrap the MCObject in a TrackObject
        return new TrackObject(category, role, object);
    }
    else if (role == "wall" || role == "wallLong")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setIsStationary(true);
        data.setSurfaceId(role.toStdString());
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
