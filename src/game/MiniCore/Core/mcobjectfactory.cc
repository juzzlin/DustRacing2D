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
#include "mcrectshape.hh"
#include "mcsurface.hh"
#include "mcsurfacemanager.hh"
#include "mcsurfaceview.hh"

#include <cassert>
#include <memory>
#include <vector>

class MCObject;
class MCObjectData;

MCObjectFactory::MCObjectFactory(MCAssetManager & assetManager)
: m_assetManager(assetManager)
{
}

MCObject & MCObjectFactory::build(const MCSurfaceObjectData & data)
{
    MCShape     * pShape   = nullptr;
    MCShapeView * pView    = nullptr;
    MCObject    * pObject  = nullptr;

    MCSurface   & surface  = m_assetManager.surfaceManager().surface(data.surfaceId());

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::None:
        // Circle shape according to surface dimensions
        if (data.defaultCirleShape())
        {
            pObject = new MCObject(data.typeId());
            pView   = new MCSurfaceView(data.typeId(), &surface, data.batchMode());
            pShape  = new MCCircleShape(pView,
                std::max(surface.width(), surface.height()) / 2);
            pObject->setShape(pShape);
        }
        // Rect shape according to surface dimensions (default)
        else
        {
            pObject = new MCObject(&surface, data.typeId());
        }
        break;

    // Explicit circle shape
    case MCObjectData::Circle:
        pObject = new MCObject(data.typeId());
        pView   = new MCSurfaceView(data.typeId(), &surface, data.batchMode());
        pShape  = new MCCircleShape(pView, data.shapeRadius());
        pObject->setShape(pShape);
        break;

    // Explicit rect shape
    case MCObjectData::Rect:
        pObject = new MCObject(data.typeId());
        pView   = new MCSurfaceView(data.typeId(), &surface);
        pShape  = new MCRectShape(pView, data.shapeWidth(), data.shapeHeight());
        pObject->setShape(pShape);
        break;
    }

    assert(pObject);
    setCommonProperties(*pObject, data);

    // Store for deletion
    m_objects.push_back(std::shared_ptr<MCObject>(pObject));

    return *pObject;
}

MCObject & MCObjectFactory::build(const MCMeshObjectData & data)
{
    MCShape     * pShape   = nullptr;
    MCShapeView * pView    = nullptr;
    MCObject    * pObject  = nullptr;

    MCMesh      & mesh     = m_assetManager.meshManager().mesh(data.meshId());

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::None:
        // Circle shape according to surface dimensions
        if (data.defaultCirleShape())
        {
            pObject = new MCObject(data.typeId());
            pView   = new MCMeshView(data.typeId(), &mesh, data.batchMode());
            pShape  = new MCCircleShape(pView, std::max(mesh.width(), mesh.height()) / 2);
            pObject->setShape(pShape);
        }
        // Rect shape according to mesh dimensions (default)
        else
        {
            pObject = new MCObject(data.typeId());
            pView   = new MCMeshView(data.typeId(), &mesh, data.batchMode());
            pShape  = new MCRectShape(pView, mesh.width(), mesh.height());
            pObject->setShape(pShape);
        }
        break;

    // Explicit circle shape
    case MCObjectData::Circle:
        pObject = new MCObject(data.typeId());
        pView   = new MCMeshView(data.typeId(), &mesh, data.batchMode());
        pShape  = new MCCircleShape(pView, data.shapeRadius());
        pObject->setShape(pShape);
        break;

    // Explicit rect shape
    case MCObjectData::Rect:
        pObject = new MCObject(data.typeId());
        pView   = new MCMeshView(data.typeId(), &mesh);
        pShape  = new MCRectShape(pView, data.shapeWidth(), data.shapeHeight());
        pObject->setShape(pShape);
        break;
    }

    assert(pObject);
    setCommonProperties(*pObject, data);

    // Store for deletion
    m_objects.push_back(std::shared_ptr<MCObject>(pObject));

    return *pObject;
}

MCObject & MCObjectFactory::build(const MCObjectData & data, MCShapeView & view)
{
    MCShape     * pShape   = nullptr;
    MCShapeView * pView    = &view;
    MCObject    * pObject  = nullptr;

    switch (data.shape())
    {
    // Default shape, use surface dimensions
    case MCObjectData::None:
        // Auto shape not supported.
        break;

    // Explicit circle shape
    case MCObjectData::Circle:
        pObject = new MCObject(data.typeId());
        pShape  = new MCCircleShape(pView, data.shapeRadius());
        pObject->setShape(pShape);
        break;

    // Explicit rect shape
    case MCObjectData::Rect:
        pObject = new MCObject(data.typeId());
        pShape  = new MCRectShape(pView, data.shapeWidth(), data.shapeHeight());
        pObject->setShape(pShape);
        break;
    }

    assert(pObject);
    setCommonProperties(*pObject, data);

    // Store for deletion
    m_objects.push_back(std::shared_ptr<MCObject>(pObject));

    return *pObject;
}

void MCObjectFactory::setCommonProperties(
    MCObject & object, const MCObjectData & data) const
{
    object.setMass(data.mass(), data.stationary());
    object.setXYFriction(data.xyFriction());
    object.setRestitution(data.restitution());
    object.setLayer(data.layer(), false);
    object.setInitialLocation(data.initialLocation());
    object.setInitialAngle(data.initialAngle());
}

MCObjectFactory::~MCObjectFactory()
{
}
