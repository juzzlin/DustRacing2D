// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcworld.hh"
#include "mcworldimpl.hh"
#include "mcbbox.hh"
#include "mcmathutil.hh"
#include "mcobject.hh"
#include "mcobjecttree.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mcrectshape.hh"
#include "mctrigonom.hh"
#include "mccamera.hh"
#include "Physics/mcforcegenerator.hh"
#include "Physics/mcfrictiongenerator.hh"

#include <GL/gl.h>

#include <cassert>

MCWorld * MCWorldImpl::pInstance             = nullptr;
MCFloat   MCWorldImpl::metersPerPixel        = 1.0;
MCFloat   MCWorldImpl::metersPerPixelSquared = 1.0;

namespace
{
// Set dimensions for minimum objectTree leaves
const MCFloat MinLeafWidth  = 64;
const MCFloat MinLeafHeight = 64;
}

MCWorldImpl::MCWorldImpl()
: pObjectTree(nullptr)
, minX(0)
, maxX(0)
, minY(0)
, maxY(0)
, minZ(0)
, maxZ(0)
, pLeft(nullptr)
, pRight(nullptr)
, pTop(nullptr)
, pBottom(nullptr)
, numCollisions(0)
, numResolverLoops(5)
, resolverStep(1.0f / numResolverLoops)
{
    for (unsigned i = 0; i < MCWorld::MaxLayers; i++)
    {
        depthTestEnabled[i] = false;
    }
}

MCWorldImpl::~MCWorldImpl()
{
    delete pLeft;
    delete pRight;
    delete pTop;
    delete pBottom;
}

void MCWorldImpl::integrate(MCFloat step)
{
    // Integrate and update all registered objects
    forceRegistry.update();
    const MCUint i2 = objs.size();
    for (MCUint i = 0; i < i2; i++)
    {
        MCObject & object(*objs[i]);
        if (object.physicsObject() && !object.stationary())
        {
            object.integrate(step);
        }

        object.stepTime();
    }
}

void MCWorldImpl::detectCollisions()
{
    // Check collisions for all registered objects
    numCollisions = 0;
    static MCObjectTree::ObjectSet possibleCollisions;
    for (MCUint i = 0; i < objs.size(); i++)
    {
        MCObject & object(*objs[i]);
        if (object.physicsObject() && !object.bypassCollisions())
        {
            possibleCollisions.clear();
            object.deleteContacts();
            pObjectTree->getBBoxCollisions(object, possibleCollisions);
            auto j1 = possibleCollisions.begin();
            auto j2 = possibleCollisions.end();
            while (j1 != j2)
            {
                if (collisionDetector.processPossibleCollision(object, **j1))
                {
                    numCollisions++;
                }
                j1++;
            }
        }
    }
}

void MCWorldImpl::generateImpulses()
{
    for (MCObject * object : objs)
    {
        if (object->physicsObject())
        {
            impulseGenerator.generateImpulsesFromDeepestContacts(*object);
        }
    }
}

void MCWorldImpl::resolvePositions(MCFloat accuracy)
{
    for (MCObject * object : objs)
    {
        if (object->physicsObject())
        {
            impulseGenerator.resolvePosition(*object, accuracy);
        }
    }
}

void MCWorldImpl::render(MCCamera * pCamera)
{
    // Render in the order of the layers. Depth test is
    // layer-specific.

    static LayerHash::iterator j;
    static LayerHash::iterator end;

    glPushAttrib(GL_ENABLE_BIT);

    for (MCUint i = 0; i < MCWorld::MaxLayers; i++)
    {
        if (depthTestEnabled[i])
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        j   = layers[i].begin();
        end = layers[i].end();
        for (; j != end; j++)
        {
            MCObject & object = **j;
            if (object.renderable())
            {
                // Check if view is set and is visible
                if (object.shape() && object.shape()->view())
                {
                    MCBBox<MCFloat> bbox(object.shape()->view()->bbox());
                    bbox.translate(MCVector2dF(object.location()));
                    if (!pCamera || pCamera->isVisible(bbox))
                    {
                        object.render(pCamera); // pCamera can be a nullptr
                    }
                }
                // Check if shape is visible
                else if (!pCamera || pCamera->isVisible(object.bbox()))
                {
                    object.render(pCamera); // pCamera can be a nullptr
                }
            }
            else if (object.virtualObject())
            {
                object.render(pCamera);
            }
        }
    }

    glPopAttrib();
}

void MCWorldImpl::renderShadows(MCCamera * pCamera)
{
    const MCUint i2 = objs.size();
    for (MCUint i = 0; i < i2; i++)
    {
        MCObject & object = *objs[i];
        if (object.renderable() && object.hasShadow())
        {
            // Check if view is set and is visible
            if (object.shape() && object.shape()->view())
            {
                MCBBox<MCFloat> bbox(object.shape()->view()->bbox());
                bbox.translate(MCVector2dF(object.location()));
                if (!pCamera || pCamera->isVisible(bbox))
                {
                    object.renderShadow(pCamera); // pCamera can be a nullptr
                }
            }
            // Check if shape is visible
            else if (!pCamera || pCamera->isVisible(object.bbox()))
            {
                object.renderShadow(pCamera); // pCamera can be a nullptr
            }
        }
        else if (object.virtualObject())
        {
            object.renderShadow(pCamera);
        }
    }
}

MCWorld::MCWorld()
: m_pImpl(new MCWorldImpl)
{
    if (!MCWorldImpl::pInstance)
    {
        MCWorldImpl::pInstance = this;
    }
    else
    {
        std::cerr << "ERROR!!: Only one MCWorld can exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Default dimensions. Creates also MCObjectTree.
    setDimensions(0.0f, MinLeafWidth, 0.0f, MinLeafHeight, 0.0f, 1.0f, 1.0f);
}

MCWorld & MCWorld::instance()
{
    if (!MCWorldImpl::pInstance)
    {
        std::cerr << "ERROR!!: MCWorld instance not created!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return *MCWorldImpl::pInstance;
}

void MCWorld::setDimensions(
    MCFloat minX, MCFloat maxX, MCFloat minY, MCFloat maxY, MCFloat minZ, MCFloat maxZ,
    MCFloat metersPerPixel)
{
    // Init objectTree
    delete m_pImpl->pObjectTree;
    m_pImpl->pObjectTree = new MCObjectTree(
        minX, minY, maxX, maxY, MinLeafWidth, MinLeafHeight);

    // Set dimensions
    m_pImpl->minX = minX;
    m_pImpl->maxX = maxX;
    m_pImpl->minY = minY;
    m_pImpl->maxY = maxY;
    m_pImpl->minZ = minZ;
    m_pImpl->maxZ = maxZ;

    MCWorld::setMetersPerPixel(metersPerPixel);

    // Create "wall" objects
    const MCFloat w = maxX - minX;
    const MCFloat h = maxY - minY;

    if (m_pImpl->pLeft)
    {
        removeObjectNow(*m_pImpl->pLeft);
        delete m_pImpl->pLeft;
    }

    m_pImpl->pLeft = new MCObject("LEFT_WALL");
    m_pImpl->pLeft->setShape(new MCRectShape(nullptr, w, h));
    m_pImpl->pLeft->setMass(0, true);
    m_pImpl->pLeft->setRestitution(0.25f);
    m_pImpl->pLeft->addToWorld();
    m_pImpl->pLeft->translate(MCVector3dF(-w / 2, h / 2, 0));

    if (m_pImpl->pRight)
    {
        removeObjectNow(*m_pImpl->pRight);
        delete m_pImpl->pRight;
    }

    m_pImpl->pRight = new MCObject("RIGHT_WALL");
    m_pImpl->pRight->setShape(new MCRectShape(nullptr, w, h));
    m_pImpl->pRight->setMass(0, true);
    m_pImpl->pRight->setRestitution(0.25f);
    m_pImpl->pRight->addToWorld();
    m_pImpl->pRight->translate(MCVector3dF(w + w / 2, h / 2, 0));

    if (m_pImpl->pTop)
    {
        removeObjectNow(*m_pImpl->pTop);
        delete m_pImpl->pTop;
    }

    m_pImpl->pTop = new MCObject("TOP_WALL");
    m_pImpl->pTop->setShape(new MCRectShape(nullptr, w, h));
    m_pImpl->pTop->setMass(0, true);
    m_pImpl->pTop->setRestitution(0.25f);
    m_pImpl->pTop->addToWorld();
    m_pImpl->pTop->translate(MCVector3dF(w / 2, h + h / 2, 0));

    if (m_pImpl->pBottom)
    {
        removeObjectNow(*m_pImpl->pBottom);
        delete m_pImpl->pBottom;
    }

    m_pImpl->pBottom = new MCObject("BOTTOM_WALL");
    m_pImpl->pBottom->setShape(new MCRectShape(nullptr, w, h));
    m_pImpl->pBottom->setMass(0, true);
    m_pImpl->pBottom->setRestitution(0.25f);
    m_pImpl->pBottom->addToWorld();
    m_pImpl->pBottom->translate(MCVector3dF(w / 2, -h / 2, 0));
}

MCFloat MCWorld::minX() const
{
    return m_pImpl->minX;
}

MCFloat MCWorld::maxX() const
{
    return m_pImpl->maxX;
}

MCFloat MCWorld::minY() const
{
    return m_pImpl->minY;
}

MCFloat MCWorld::maxY() const
{
    return m_pImpl->maxY;
}

MCFloat MCWorld::minZ() const
{
    return m_pImpl->minZ;
}

MCFloat MCWorld::maxZ() const
{
    return m_pImpl->maxZ;
}

void MCWorld::addObject(MCObject & object)
{
    m_pImpl->addObject(object);
}

void MCWorldImpl::addObject(MCObject & object)
{
    if (!object.removing())
    {
        if (object.index() == -1)
        {
            // Add to layer map
            addToLayerMap(object);

            // Add to object vector (O(1))
            objs.push_back(&object);
            object.setIndex(objs.size() - 1);

            // Add to ObjectTree
            if (object.physicsObject() && !object.bypassCollisions())
            {
                pObjectTree->insert(object);
            }

            // Add xy friction
            const MCFloat FrictionThreshold = 0.001f;
            if (object.xyFriction() > FrictionThreshold)
            {
                forceRegistry.addForceGenerator(
                    *new MCFrictionGenerator(
                        object.xyFriction(), object.xyFriction()), object, true);
            }
        }
    }
    else
    {
        object.setRemoving(false);
    }
}

void MCWorld::removeObject(MCObject & object)
{
    object.setRemoving(true);
    m_pImpl->removeObjs.push_back(&object);
}

void MCWorld::removeObjectNow(MCObject & object)
{
    object.setRemoving(true);
    for (MCObject * obj : m_pImpl->objs)
    {
        if (obj != &object)
        {
            if (obj->physicsObject())
            {
                obj->deleteContacts(object);
            }
        }
    }

    m_pImpl->removeObject(object);
}

void MCWorldImpl::removeObject(MCObject & object)
{
    // Reset motion
    object.resetMotion();

    // Remove from the layer map
    removeFromLayerMap(object);

    // Remove from object vector (O(1))
    if (object.index() > -1 && object.index() < static_cast<int>(objs.size()))
    {
        objs[object.index()] = objs.back();
        objs[object.index()]->setIndex(object.index());
        objs.pop_back();
        object.setIndex(-1);
    }

    // Remove from ObjectTree
    if (object.physicsObject() && !object.bypassCollisions())
    {
        pObjectTree->remove(object);
    }

    object.setRemoving(false);
}

void MCWorldImpl::processRemovedObjects()
{
    for (MCObject * obj : removeObjs)
    {
        if (obj->removing())
        {
            removeObject(*obj);
        }
    }

    removeObjs.clear();
}

void MCWorldImpl::addToLayerMap(MCObject & object)
{
    const MCUint layerIndex =
        object.layer() >= MCWorld::MaxLayers ? MCWorld::MaxLayers - 1 : object.layer();
    layers[layerIndex].insert(&object);
}

void MCWorldImpl::removeFromLayerMap(MCObject & object)
{
    const MCUint layerIndex =
        object.layer() >= MCWorld::MaxLayers ? MCWorld::MaxLayers - 1 : object.layer();
    layers[layerIndex].erase(&object);
}

void MCWorld::addToLayerMap(MCObject & object)
{
    m_pImpl->addToLayerMap(object);
}

void MCWorld::removeFromLayerMap(MCObject & object)
{
    m_pImpl->removeFromLayerMap(object);
}

void MCWorld::enableDepthTestOnLayer(MCUint layer, bool enable)
{
    if (layer < MCWorld::MaxLayers)
    {
        m_pImpl->depthTestEnabled[layer] = enable;
    }
}

void MCWorld::addForceGenerator(
    MCForceGenerator & gen, MCObject & obj, bool takeOwnership)
{
    m_pImpl->forceRegistry.addForceGenerator(gen, obj, takeOwnership);
}

void MCWorld::removeForceGenerator(MCForceGenerator & gen, MCObject & obj)
{
    m_pImpl->forceRegistry.removeForceGenerator(gen, obj);
}

void MCWorld::stepTime(MCFloat step)
{
    m_pImpl->stepTime(step);
}

void MCWorldImpl::stepTime(MCFloat step)
{
    // Integrate physics
    integrate(step);
    detectCollisions();

    if (numCollisions)
    {
        generateImpulses();

        // Process contacts and generate impulses
        collisionDetector.enableCollisionEvents(false);
        for (MCUint i = 0; i < numResolverLoops; i++)
        {
            detectCollisions();
            resolvePositions(resolverStep);
        }
        collisionDetector.enableCollisionEvents(true);
    }

    // Remove objects that are marked to be removed
    processRemovedObjects();
}

void MCWorld::render(MCCamera * pCamera)
{
    m_pImpl->render(pCamera);
}

void MCWorld::renderShadows(MCCamera * pCamera)
{
    m_pImpl->renderShadows(pCamera);
}

MCWorld::ObjectVector MCWorld::objects() const
{
    return m_pImpl->objs;
}

MCObjectTree & MCWorld::objectTree() const
{
    assert(m_pImpl->pObjectTree);
    return *m_pImpl->pObjectTree;
}

void MCWorld::setMetersPerPixel(MCFloat value)
{
    MCWorldImpl::metersPerPixel        = value;
    MCWorldImpl::metersPerPixelSquared = value * value;
}

void MCWorld::toMeters(MCFloat & pixels)
{
    pixels *= MCWorldImpl::metersPerPixel;
}

void MCWorld::toMeters(MCVector2dF & pixels)
{
    pixels *= MCWorldImpl::metersPerPixel;
}

void MCWorld::toMeters(MCVector3dF & pixels)
{
    pixels *= MCWorldImpl::metersPerPixel;
}

MCWorld::~MCWorld()
{
    MCWorldImpl::pInstance = nullptr;

    delete m_pImpl->pObjectTree;
    delete m_pImpl;
}
