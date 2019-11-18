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
#include <MCObject>
#include <MCObjectFactory>
#include <MCPhysicsComponent>
#include <MCRandom>
#include <MCShape>
#include <MCShapeView>
#include <MCSurface>

#include "simple_logger.hpp"

namespace {
static const float DEFAULT_DIFFUSE_COEFF = 1.5f;
}

TrackObjectFactory::TrackObjectFactory(MCObjectFactory & objectFactory)
  : m_objectFactory(objectFactory)
{
}

TrackObject * TrackObjectFactory::build(
  QString category, QString role, MCVector2dF location, int angle, bool forceStationary)
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
        object->shape()->view()->object()->material()->setDiffuseCoeff(DEFAULT_DIFFUSE_COEFF);
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
        data.setInitialLocation(MCVector3dF(location.i(), location.j(), 0));
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setMass(1000);
        data.setMeshId(role.toStdString());
        data.setSurfaceId(role.toStdString());
        data.setRestitution(0.9f);

        object = m_objectFactory.build(data);
        object->shape()->view()->object()->material()->setDiffuseCoeff(DEFAULT_DIFFUSE_COEFF);
    }
    else if (role == "dustRacing2DBanner")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);

        data.setMass(100000);
        data.setSurfaceId(role.toStdString());

        object = m_objectFactory.build(data);
        object->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));
        object->shape()->view()->object()->material()->setDiffuseCoeff(DEFAULT_DIFFUSE_COEFF);
    }
    else if (role == "grandstand")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setRestitution(0.5);
        data.setMass(100000);
        data.setSurfaceId(role.toStdString());

        object = m_objectFactory.build(data);
        object->shape()->view()->object()->material()->setDiffuseCoeff(DEFAULT_DIFFUSE_COEFF);
    }
    else if (role == "plant")
    {
        const float plantBodyRadius = 4;

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
        data.setRestitution(0.9f);

        object = m_objectFactory.build(data);
    }
    else if (
      role == "grid" || role == "sandAreaCurve" || role == "sandAreaBig")
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
        object = std::make_shared<Pit>(MCAssetManager::surfaceManager().surface("pit"));
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
    }
    else if (role == "tree")
    {
        const int height = static_cast<int>(200 + 200 * MCRandom::getValue());
        object = std::make_shared<Tree>(MCAssetManager::surfaceManager().surface("tree"),
                                        1.0f + 0.50f * MCRandom::getValue(),
                                        0.1f + 0.2f * MCRandom::getValue(),
                                        height,
                                        height / 10);
        object->setInitialLocation(location);
        object->setInitialAngle(angle);
    }
    else if (role == "wall" || role == "wallLong")
    {
        MCSurfaceObjectData data(role.toStdString());
        data.setInitialLocation(location);
        data.setInitialAngle(angle);
        data.setXYFriction(1.0);
        data.setIsStationary(true);
        data.setRestitution(0.9f);
        data.setInitialLocation(MCVector3dF(location.i(), location.j(), 8));

        object = m_objectFactory.build(data);
        object->shape()->view()->setShaderProgram(Renderer::instance().program("defaultSpecular"));
    }

    if (!object)
    {
        juzzlin::L().warning() << "Unknown or deprecated object '" << role.toStdString() << "'";
        return nullptr;
    }
    else
    {
        // Override mass if object is forced as stationary
        if (forceStationary)
        {
            object->physicsComponent().setMass(0, true);
        }

        // Wrap the MCObject in a TrackObject
        return new TrackObject(category, role, object);
    }
}
