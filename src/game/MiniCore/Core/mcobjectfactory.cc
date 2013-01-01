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
#include "mccircleshape.hh"
#include "mcrectshape.hh"
#include "mcsurface.hh"
#include "mcsurfacemanager.hh"
#include "mcsurfaceview.hh"

#include <cassert>
#include <memory>
#include <vector>

class MCObject;
class MCObjectData;

//! Implementation class for MCObjectFactory.
class MCObjectFactoryImpl
{
private:
    MCObjectFactoryImpl(MCSurfaceManager & tm);
    void setCommonProperties(
        MCObject & object, const MCObjectData & data) const;
    std::vector<std::shared_ptr<MCObject> > objects;
    MCSurfaceManager & textureManager;
    friend class MCObjectFactory;
};

MCObjectFactory::MCObjectFactory(MCSurfaceManager & tm)
: m_pImpl(new MCObjectFactoryImpl(tm))
{
}

MCObjectFactoryImpl::MCObjectFactoryImpl(MCSurfaceManager & tm)
: textureManager(tm)
{
}

MCObject & MCObjectFactory::build(const MCSurfaceObjectData & data)
{
    MCShape     * pShape   = nullptr;
    MCShapeView * pView    = nullptr;
    MCObject    * pObject  = nullptr;

    MCSurface   & surface  = m_pImpl->textureManager.surface(data.surfaceId());

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
    m_pImpl->setCommonProperties(*pObject, data);

    // Store for deletion
    m_pImpl->objects.push_back(std::shared_ptr<MCObject>(pObject));

    return *pObject;
}

//MCObject & MCObjectFactory::build(const MCGLObjectData & data)
//{
//    return nullptr;
//}

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
        pShape  = new MCRectShape(pView,
            data.shapeWidth(), data.shapeHeight());
        pObject->setShape(pShape);
        break;
    }

    assert(pObject);
    m_pImpl->setCommonProperties(*pObject, data);

    // Store for deletion
    m_pImpl->objects.push_back(std::shared_ptr<MCObject>(pObject));

    return *pObject;
}

void MCObjectFactoryImpl::setCommonProperties(
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
    delete m_pImpl;
}
