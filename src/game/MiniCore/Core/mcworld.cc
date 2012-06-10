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

MCWorld * MCWorld::pInstance               = nullptr;
MCFloat   MCWorld::m_metersPerPixel        = 1.0;
MCFloat   MCWorld::m_metersPerPixelSquared = 1.0;

namespace
{
// Set dimensions for minimum objectTree leaves
const MCFloat MinLeafWidth  = 64;
const MCFloat MinLeafHeight = 64;
}

MCWorld::MCWorld()
: pObjectTree(nullptr)
, m_minX(0)
, m_maxX(0)
, m_minY(0)
, m_maxY(0)
, m_minZ(0)
, m_maxZ(0)
, pLeft(nullptr)
, pRight(nullptr)
, pTop(nullptr)
, pBottom(nullptr)
, numCollisions(0)
, numResolverLoops(5)
, resolverStep(1.0 / numResolverLoops)
{
    if (!MCWorld::pInstance)
    {
        MCWorld::pInstance = this;
    }
    else
    {
        std::cerr << "ERROR!!: Only one MCWorld can exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Default dimensions. Creates also MCObjectTree.
    setDimensions(0.0, MinLeafWidth, 0.0, MinLeafHeight, 0.0, 1.0, 1.0);

    for (unsigned i = 0; i < MCWorld::MaxLayers; i++)
    {
        depthTestEnabled[i] = false;
    }
}

MCWorld::~MCWorld()
{
    delete pObjectTree;
    delete pLeft;
    delete pRight;
    delete pTop;
    delete pBottom;

    MCWorld::pInstance = nullptr;
}

void MCWorld::integrate(MCFloat step)
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

void MCWorld::detectCollisions()
{
    // Check collisions for all registered objects
    numCollisions = collisionDetector.detectCollisions(objs, *pObjectTree);
}

void MCWorld::generateImpulses()
{
    impulseGenerator.generateImpulsesFromDeepestContacts(objs);
}

void MCWorld::resolvePositions(MCFloat accuracy)
{
    impulseGenerator.resolvePositions(objs, accuracy);
}

void MCWorld::render(MCCamera * pCamera)
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
            else if (object.isParticleGroup())
            {
                object.render(pCamera);
            }
        }
    }

    glPopAttrib();
}

void MCWorld::renderShadows(MCCamera * pCamera)
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
        else if (object.isParticleGroup())
        {
            object.renderShadow(pCamera);
        }
    }
}

MCWorld & MCWorld::instance()
{
    if (!MCWorld::pInstance)
    {
        std::cerr << "ERROR!!: MCWorld instance not created!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return *MCWorld::pInstance;
}

void MCWorld::setDimensions(
    MCFloat minX, MCFloat maxX, MCFloat minY, MCFloat maxY, MCFloat minZ, MCFloat maxZ,
    MCFloat metersPerPixel)
{
    MCWorld::setMetersPerPixel(metersPerPixel);

    // Set dimensions
    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;
    m_minZ = minZ;
    m_maxZ = maxZ;

    // Init objectTree
    delete pObjectTree;
    pObjectTree = new MCObjectTree(
        m_minX, m_minY,
        m_maxX, m_maxY,
        MinLeafWidth, MinLeafHeight);

    // Create "wall" objects
    const MCFloat w = m_maxX - m_minX;
    const MCFloat h = m_maxY - m_minY;

    if (pLeft)
    {
        removeObjectNow(*pLeft);
        delete pLeft;
    }

    pLeft = new MCObject("LEFT_WALL");
    pLeft->setShape(new MCRectShape(nullptr, w, h));
    pLeft->setMass(0, true);
    pLeft->setRestitution(0.25);
    pLeft->addToWorld();
    pLeft->translate(MCVector3dF(-w / 2, h / 2, 0));

    if (pRight)
    {
        removeObjectNow(*pRight);
        delete pRight;
    }

    pRight = new MCObject("RIGHT_WALL");
    pRight->setShape(new MCRectShape(nullptr, w, h));
    pRight->setMass(0, true);
    pRight->setRestitution(0.25);
    pRight->addToWorld();
    pRight->translate(MCVector3dF(w + w / 2, h / 2, 0));

    if (pTop)
    {
        removeObjectNow(*pTop);
        delete pTop;
    }

    pTop = new MCObject("TOP_WALL");
    pTop->setShape(new MCRectShape(nullptr, w, h));
    pTop->setMass(0, true);
    pTop->setRestitution(0.25);
    pTop->addToWorld();
    pTop->translate(MCVector3dF(w / 2, h + h / 2, 0));

    if (pBottom)
    {
        removeObjectNow(*pBottom);
        delete pBottom;
    }

    pBottom = new MCObject("BOTTOM_WALL");
    pBottom->setShape(new MCRectShape(nullptr, w, h));
    pBottom->setMass(0, true);
    pBottom->setRestitution(0.25);
    pBottom->addToWorld();
    pBottom->translate(MCVector3dF(w / 2, -h / 2, 0));
}

MCFloat MCWorld::minX() const
{
    return m_minX;
}

MCFloat MCWorld::maxX() const
{
    return m_maxX;
}

MCFloat MCWorld::minY() const
{
    return m_minY;
}

MCFloat MCWorld::maxY() const
{
    return m_maxY;
}

MCFloat MCWorld::minZ() const
{
    return m_minZ;
}

MCFloat MCWorld::maxZ() const
{
    return m_maxZ;
}

void MCWorld::addObject(MCObject & object)
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
    removeObjs.push_back(&object);
}

void MCWorld::removeObjectNow(MCObject & object)
{
    object.setRemoving(true);
    for (MCObject * obj : objs)
    {
        if (obj != &object)
        {
            if (obj->physicsObject())
            {
                obj->deleteContacts(object);
            }
        }
    }

    doRemoveObject(object);
}

void MCWorld::doRemoveObject(MCObject & object)
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

void MCWorld::processRemovedObjects()
{
    for (MCObject * obj : removeObjs)
    {
        if (obj->removing())
        {
            doRemoveObject(*obj);
        }
    }

    removeObjs.clear();
}

void MCWorld::addToLayerMap(MCObject & object)
{
    const MCUint layerIndex =
        object.layer() >= MCWorld::MaxLayers ? MCWorld::MaxLayers - 1 : object.layer();
    layers[layerIndex].insert(&object);
}

void MCWorld::removeFromLayerMap(MCObject & object)
{
    const MCUint layerIndex =
        object.layer() >= MCWorld::MaxLayers ? MCWorld::MaxLayers - 1 : object.layer();
    layers[layerIndex].erase(&object);
}

void MCWorld::enableDepthTestOnLayer(MCUint layer, bool enable)
{
    if (layer < MCWorld::MaxLayers)
    {
        depthTestEnabled[layer] = enable;
    }
}

void MCWorld::addForceGenerator(
    MCForceGenerator & gen, MCObject & obj, bool takeOwnership)
{
    forceRegistry.addForceGenerator(gen, obj, takeOwnership);
}

void MCWorld::removeForceGenerator(MCForceGenerator & gen, MCObject & obj)
{
    forceRegistry.removeForceGenerator(gen, obj);
}

void MCWorld::stepTime(MCFloat step)
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

MCWorld::ObjectVector MCWorld::objects() const
{
    return objs;
}

MCObjectTree & MCWorld::objectTree() const
{
    assert(pObjectTree);
    return *pObjectTree;
}

void MCWorld::setMetersPerPixel(MCFloat value)
{
    MCWorld::m_metersPerPixel        = value;
    MCWorld::m_metersPerPixelSquared = value * value;
}

MCFloat MCWorld::metersPerPixel()
{
    assert(MCWorld::pInstance);
    return MCWorld::m_metersPerPixel;
}

void MCWorld::toMeters(MCFloat & pixels)
{
    pixels *= MCWorld::m_metersPerPixel;
}

void MCWorld::toMeters(MCVector2dF & pixels)
{
    pixels *= MCWorld::m_metersPerPixel;
}

void MCWorld::toMeters(MCVector3dF & pixels)
{
    pixels *= MCWorld::m_metersPerPixel;
}
