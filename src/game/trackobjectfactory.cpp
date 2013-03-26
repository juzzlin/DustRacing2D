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
#include "trackobject.hpp"
#include "treeview.hpp"
#include "renderer.hpp"
#include "layers.hpp"

#include <MCAssetManager>
#include <MCObject>
#include <MCObjectFactory>
#include <MCShapeView>

TrackObjectFactory::TrackObjectFactory(MCObjectFactory & objectFactory)
: m_objectFactory(objectFactory)
{
}

TrackObject & TrackObjectFactory::build(
    QString category, QString role, MCVector2dF location, int angle)
{
    MCObject * object = nullptr;

    if (role == "brake")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setMass(1000);
        data.setSurfaceId(role.toStdString());
        data.setLayer(Layers::GrandStands);

        object = &m_objectFactory.build(data);
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
        data.setLayer(Layers::Meshes);

        object = &m_objectFactory.build(data);
    }
    else if (role == "dustRacing2DBanner")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setMass(50000);
        data.setSurfaceId(role.toStdString());
        data.setLayer(Layers::GrandStands);

        object = &m_objectFactory.build(data);
    }
    else if (role == "grandstand")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setStationary(true);
        data.setSurfaceId(role.toStdString());
        data.setLayer(Layers::GrandStands);

        object = &m_objectFactory.build(data);
    }
    else if (role == "plant")
    {
        const MCFloat plantBodyRadius = 4;

        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setStationary(true);
        data.setSurfaceId(role.toStdString());
        data.setRestitution(0.25);
        data.setShapeWidth(plantBodyRadius);
        data.setShapeHeight(plantBodyRadius);
        data.setLayer(Layers::Tree);

        object = &m_objectFactory.build(data);
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
        data.setLayer(Layers::Walls);

        object = &m_objectFactory.build(data);
    }
    else if (role == "sandAreaCurve" || role == "sandAreaBig")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setStationary(true);
        data.setSurfaceId(role.toStdString());
        data.setLayer(Layers::Ground);

        object = &m_objectFactory.build(data);
        object->setPhysicsObject(false);
        object->view()->setHasShadow(false);
    }
    else if (role == "tire")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setMass(1000); // Exaggerate the mass on purpose
        data.setSurfaceId(role.toStdString());
        data.setXYFriction(0.25);
        data.setLayer(Layers::Walls);

        object = &m_objectFactory.build(data);
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
        data.setStationary(true);
        data.setRestitution(0.25);
        data.setShapeWidth(treeBodyRadius);
        data.setShapeHeight(treeBodyRadius);
        data.setLayer(Layers::Tree);

        // Create a custom view.
        MCShapeView * view = new TreeView(
            MCAssetManager::surfaceManager().surface("tree"), treeViewRadius, 2, 120, 5);
        view->setShaderProgram(&(Renderer::instance().program("master")));
        view->setShadowShaderProgram(&(Renderer::instance().program("masterShadow")));
        MCObject & object = m_objectFactory.build(data, *view);

        // Wrap the MCObject in a TrackObject
        return *new TrackObject(category, role, object);
    }
    else if (role == "wall" || role == "wallLong")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setBatchMode(true);
        data.setXYFriction(1.0);
        data.setStationary(true);
        data.setSurfaceId(role.toStdString());
        data.setRestitution(0.9);
        data.setLayer(Layers::Walls);
        data.setInitialLocation(MCVector3dF(location.i(), location.j(), 8));

        object = &m_objectFactory.build(data);
    }

    assert(object);

    object->view()->setShaderProgram(&Renderer::instance().program("master"));
    object->view()->setShadowShaderProgram(&Renderer::instance().program("masterShadow"));

    // Wrap the MCObject in a TrackObject
    return *new TrackObject(category, role, *object);
}
