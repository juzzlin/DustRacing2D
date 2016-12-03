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
    MCShapeViewPtr view;
    MCShapePtr     shape;
    MCSurface    & surface = m_assetManager.surfaceManager().surface(data.surfaceId());
    MCObject     * object  = nullptr;

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::Default:
        // Circle shape according to surface dimensions
        if (data.defaultCirleShape())
        {
            object = new MCObject(data.typeId());
            view.reset(new MCSurfaceView(data.typeId(), &surface));
            view->setBatchMode(data.batchMode());
            shape.reset(new MCCircleShape(view, std::max(surface.width(), surface.height()) / 2));
            object->setShape(shape);
        }
        // Rect shape according to surface dimensions (default)
        else
        {
            object = new MCObject(surface, data.typeId(), data.batchMode());
        }
        break;

    // Explicit circle shape
    case MCObjectData::Circle:
        object = new MCObject(data.typeId());
        view.reset(new MCSurfaceView(data.typeId(), &surface));
        view->setBatchMode(data.batchMode());
        shape.reset(new MCCircleShape(view, data.shapeRadius()));
        object->setShape(shape);
        break;

    // Explicit rect shape
    case MCObjectData::Rect:
        object = new MCObject(data.typeId());
        view.reset(new MCSurfaceView(data.typeId(), &surface));
        view->setBatchMode(data.batchMode());
        shape.reset(new MCRectShape(view, data.shapeWidth(), data.shapeHeight()));
        object->setShape(shape);
        break;
    }

    assert(object);
    setCommonProperties(*object, data);

    return MCObjectPtr(object);
}

MCObjectPtr MCObjectFactory::build(const MCMeshObjectData & data)
{
    MCShapeViewPtr view;
    MCShapePtr     shape;
    MCObject     * object = nullptr;
    MCMesh       & mesh   = m_assetManager.meshManager().mesh(data.meshId());

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::Default:
        // Circle shape according to surface dimensions
        if (data.defaultCirleShape())
        {
            object = new MCObject(data.typeId());
            view.reset(new MCMeshView(data.typeId(), &mesh));
            view->setBatchMode(data.batchMode());
            shape.reset(new MCCircleShape(view, std::max(mesh.width(), mesh.height()) / 2));
            object->setShape(shape);
        }
        // Rect shape according to mesh dimensions (default)
        else
        {
            object = new MCObject(data.typeId());
            view.reset(new MCMeshView(data.typeId(), &mesh));
            view->setBatchMode(data.batchMode());
            shape.reset(new MCRectShape(view, mesh.width(), mesh.height()));
            object->setShape(shape);
        }
        break;

    // Explicit circle shape
    case MCObjectData::Circle:
        object = new MCObject(data.typeId());
        view.reset(new MCMeshView(data.typeId(), &mesh));
        view->setBatchMode(data.batchMode());
        shape.reset(new MCCircleShape(view, data.shapeRadius()));
        object->setShape(shape);
        break;

    // Explicit rect shape
    case MCObjectData::Rect:
        object = new MCObject(data.typeId());
        view.reset(new MCMeshView(data.typeId(), &mesh));
        view->setBatchMode(data.batchMode());
        shape.reset(new MCRectShape(view, data.shapeWidth(), data.shapeHeight()));
        object->setShape(shape);
        break;
    }

    assert(object);
    setCommonProperties(*object, data);

    return MCObjectPtr(object);
}

MCObjectPtr MCObjectFactory::build(const MCObjectData & data, MCShapeViewPtr view)
{
    MCShapePtr shape;
    MCObject * object = nullptr;

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::Default:
        // Auto shape not supported.
        break;

    // Explicit circle shape
    case MCObjectData::Circle:
        object = new MCObject(data.typeId());
        shape.reset(new MCCircleShape(view, data.shapeRadius()));
        object->setShape(shape);
        break;

    // Explicit rect shape
    case MCObjectData::Rect:
        object = new MCObject(data.typeId());
        shape.reset(new MCRectShape(view, data.shapeWidth(), data.shapeHeight()));
        object->setShape(shape);
        break;
    }

    assert(object);
    setCommonProperties(*object, data);

    return MCObjectPtr(object);
}

void MCObjectFactory::setCommonProperties(
    MCObject & object, const MCObjectData & data) const
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
