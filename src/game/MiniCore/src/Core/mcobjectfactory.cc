// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcobjectfactory.hh"

#include "mcassetmanager.hh"
#include "mccircleshape.hh"
#include "mcmeshview.hh"
#include "mcphysicscomponent.hh"
#include "mcrectshape.hh"
#include "mcsurface.hh"
#include "mcsurfacemanager.hh"
#include "mcsurfaceview.hh"

#include <cassert>
#include <memory>
#include <vector>

MCObjectFactory::MCObjectFactory(MCAssetManager & assetManager)
  : m_assetManager(assetManager)
{
}

MCObjectPtr MCObjectFactory::build(const MCSurfaceObjectData & data)
{
    const auto surface = m_assetManager.surfaceManager().surface(data.surfaceId());

    MCObjectPtr object;

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::Default:
    {
        // Circle shape according to surface dimensions
        if (data.defaultCircleShape())
        {
            object = std::make_shared<MCObject>(data.typeId());
            object->setShape(std::make_shared<MCCircleShape>(
              std::make_shared<MCSurfaceView>(data.typeId(), surface), std::max(surface->width(), surface->height()) / 2));
        }
        // Rect shape according to surface dimensions (default)
        else
        {
            object = std::make_shared<MCObject>(surface, data.typeId());
        }
        break;
    }
    // Explicit circle shape
    case MCObjectData::Circle:
    {
        object = std::make_shared<MCObject>(data.typeId());
        object->setShape(std::make_shared<MCCircleShape>(
          std::make_shared<MCSurfaceView>(data.typeId(), surface), data.shapeRadius()));
        break;
    }
    // Explicit rect shape
    case MCObjectData::Rect:
    {
        object = std::make_shared<MCObject>(data.typeId());
        object->setShape(std::make_shared<MCRectShape>(
          std::make_shared<MCSurfaceView>(data.typeId(), surface), data.shapeWidth(), data.shapeHeight()));
        break;
    }
    }
    assert(object);
    setCommonProperties(*object, data);

    return MCObjectPtr(object);
}

MCObjectPtr MCObjectFactory::build(const MCMeshObjectData & data)
{
    const auto object = std::make_shared<MCObject>(data.typeId());
    const auto mesh = m_assetManager.meshManager().mesh(data.meshId());

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::Default:
        // Circle shape according to surface dimensions
        if (data.defaultCircleShape())
        {
            object->setShape(std::make_shared<MCCircleShape>(std::make_shared<MCMeshView>(data.typeId(), mesh), std::max(mesh->width(), mesh->height()) / 2));
        }
        // Rect shape according to mesh dimensions (default)
        else
        {
            object->setShape(std::make_shared<MCRectShape>(std::make_shared<MCMeshView>(data.typeId(), mesh), mesh->width(), mesh->height()));
        }
        break;

    // Explicit circle shape
    case MCObjectData::Circle:
        object->setShape(std::make_shared<MCCircleShape>(std::make_shared<MCMeshView>(data.typeId(), mesh), data.shapeRadius()));
        break;

    // Explicit rect shape
    case MCObjectData::Rect:
        object->setShape(std::make_shared<MCRectShape>(std::make_shared<MCMeshView>(data.typeId(), mesh), data.shapeWidth(), data.shapeHeight()));
        break;
    }

    setCommonProperties(*object, data);

    return object;
}

MCObjectPtr MCObjectFactory::build(const MCObjectData & data, MCShapeViewPtr view)
{
    MCObjectPtr object;

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::Default:
        // Auto shape not supported.
        return nullptr;

    // Explicit circle shape
    case MCObjectData::Circle:
    {
        object = std::make_shared<MCObject>(data.typeId());
        object->setShape(std::make_shared<MCCircleShape>(view, data.shapeRadius()));
        break;
    }
    // Explicit rect shape
    case MCObjectData::Rect:
    {
        object = std::make_shared<MCObject>(data.typeId());
        object->setShape(std::make_shared<MCRectShape>(view, data.shapeWidth(), data.shapeHeight()));
        break;
    }
    }

    setCommonProperties(*object, data);

    return object;
}

void MCObjectFactory::setCommonProperties(MCObject & object, const MCObjectData & data) const
{
    object.physicsComponent().setMass(data.mass(), data.stationary());
    object.physicsComponent().setXYFriction(data.xyFriction());
    object.physicsComponent().setRestitution(data.restitution());

    object.setInitialLocation(data.initialLocation());
    object.setInitialAngle(data.initialAngle());
}

MCObjectFactory::~MCObjectFactory()
{
}
