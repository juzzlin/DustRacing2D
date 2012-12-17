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
#include "mcparticle.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mcrectshape.hh"
#include "mctrigonom.hh"
#include "mccamera.hh"
#include "mcforcegenerator.hh"
#include "mcfrictiongenerator.hh"

#include <MCGLEW>
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
: m_objectTree(nullptr)
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
        m_depthTestEnabled[i] = false;
    }
}

MCWorld::~MCWorld()
{
    clear();

    delete m_objectTree;
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
    numCollisions = collisionDetector.detectCollisions(*m_objectTree);
}

void MCWorld::generateImpulses()
{
    impulseGenerator.generateImpulsesFromDeepestContacts(objs);
}

void MCWorld::resolvePositions(MCFloat accuracy)
{
    impulseGenerator.resolvePositions(objs, accuracy);
}

void MCWorld::render(MCCamera * pCamera, bool enableShadows)
{
    buildBatches(pCamera);

    if (enableShadows)
    {
        renderShadows(pCamera);
    }

    renderBatches(pCamera);
}

void MCWorld::buildBatches(MCCamera * pCamera)
{
    // In the case of Dust Racing 2D, it was faster to just loop through
    // all objects on all layers and perform visibility tests instead of
    // just fetching all "visible" objects from MCObjectTree.

    // This code tests the visibility and sorts the objects with respect
    // to their view id's into "batches". MCWorld::render()
    // (and MCWorld::renderShadows()) then goes through these batches
    // and perform the actual rendering.

    // Grouping the objects like this reduces texture switches etc and increases
    // overall performance.

    for (MCUint i = 0; i < MCWorld::MaxLayers; i++)
    {
        m_objectBatches[i].clear();
        m_particleBatches[i].clear();

        LayerHash::iterator j = layers[i].begin();
        const LayerHash::iterator end = layers[i].end();

        for (; j != end; j++)
        {
            MCObject & object = **j;
            if (object.renderable())
            {
                // Check if view is set and is visible
                if (object.shape())
                {
                    if (!object.isParticle())
                    {
                        if (object.shape()->view())
                        {
                            MCBBox<MCFloat> bbox(object.shape()->view()->bbox());
                            bbox.translate(MCVector2dF(object.location()));
                            if (!pCamera || pCamera->isVisible(bbox))
                            {
                                m_objectBatches[i][object.typeID()].push_back(&object);
                            }
                        }
                    }
                    else
                    {
                        if (pCamera)
                        {
                            if (pCamera->isVisible(object.bbox()))
                            {
                                m_particleBatches[i][object.typeID()].push_back(&object);
                            }
                            else
                            {
                                // Optimization that kills non-visible particles.
                                bool isVisibleInAnyCamera = false;
                                for (MCCamera * camera : m_visibilityCameras)
                                {
                                    if (camera != pCamera && camera->isVisible(object.bbox()))
                                    {
                                        isVisibleInAnyCamera = true;
                                        break;
                                    }
                                }

                                if (!isVisibleInAnyCamera)
                                {
                                    static_cast<MCParticle &>(object).die();
                                }
                            }
                        }
                        else
                        {
                            m_particleBatches[i][object.typeID()].push_back(&object);
                        }
                    }
                }
            }
        }
    }
}

void MCWorld::renderBatches(MCCamera * pCamera)
{
    // Render in the order of the layers. Depth test is
    // layer-specific.
    glPushAttrib(GL_ENABLE_BIT);

    for (MCUint layer = 0; layer < MCWorld::MaxLayers; layer++)
    {
        // The depth test is enabled/disabled separately on
        // each object layer.
        if (m_depthTestEnabled[layer])
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        renderObjectBatches(pCamera, layer);
        renderParticleBatches(pCamera, layer);
    }

    glPopAttrib();
}

void MCWorld::renderObjectBatches(MCCamera * pCamera, int layer)
{
    auto iter = m_objectBatches[layer].begin();
    const auto end = m_objectBatches[layer].end();
    while (iter != end)
    {
        const int i2 = iter->second.size();
        for (int i = 0; i < i2; i++)
        {
            MCObject    * object = iter->second[i];
            MCShapeView * view = object->shape()->view();

            if (i == 0)
            {
                view->beginBatch();
            }

            object->render(pCamera);

            if (i == i2 - 1)
            {
                view->endBatch();
            }
        }

        iter++;
    }
}

void MCWorld::renderParticleBatches(MCCamera * pCamera, int layer)
{
    // Render particle batches
    auto iter = m_particleBatches[layer].begin();
    const auto end = m_particleBatches[layer].end();
    while (iter != end)
    {
        const int i2 = iter->second.size();
        for (int i = 0; i < i2; i++)
        {
            MCParticle * particle = static_cast<MCParticle *>(iter->second[i]);

            if (i == 0)
            {
                particle->beginBatch();
            }

            particle->render(pCamera);

            if (i == i2 - 1)
            {
                particle->endBatch();
            }
        }

        iter++;
    }
}

void MCWorld::renderShadows(MCCamera * pCamera)
{
    glPushAttrib(GL_ENABLE_BIT);

    for (MCUint i = 0; i < MCWorld::MaxLayers; i++)
    {
        glDisable(GL_DEPTH_TEST);

        // Render batches
        auto iter = m_objectBatches[i].begin();
        const auto end = m_objectBatches[i].end();
        while (iter != end)
        {
            const int i2 = iter->second.size();
            for (int i = 0; i < i2; i++)
            {
                MCObject * object = iter->second[i];

                if (object->hasShadow())
                {
                    MCShapeView * view = object->shape()->view();

                    if (i == 0)
                    {
                        view->beginShadowBatch();
                    }

                    object->renderShadow(pCamera);

                    if (i == i2 - 1)
                    {
                        view->endShadowBatch();
                    }
                }
            }

            iter++;
        }
    }

    glPopAttrib();
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

void MCWorld::clear()
{
    // This does the same as removeObject(), but the removal
    // process here is simpler as all data structures will be
    // cleared and all objects will be removed at once.
    for (MCObject * object : objs)
    {
        object->deleteContacts();
        object->resetMotion();
        object->setIndex(-1);

        if (object->isParticle())
        {
            static_cast<MCParticle *>(object)->die();
        }
    }

    m_objectTree->removeAll();
    objs.clear();
    removeObjs.clear();

    for (unsigned int i = 0; i < MCWorld::MaxLayers; i++)
    {
        layers[i].clear();
    }
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
    delete m_objectTree;
    m_objectTree = new MCObjectTree(
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
                m_objectTree->insert(object);
            }

            // Add xy friction
            const MCFloat FrictionThreshold = 0.001;
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
    if (object.index() >= 0)
    {
        object.setRemoving(true);
        removeObjs.push_back(&object);
    }
}

void MCWorld::removeObjectNow(MCObject & object)
{
    if (object.index() >= 0)
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
        m_objectTree->remove(object);
    }

    object.setRemoving(false);
}

void MCWorld::removeObjectFromIntegration(MCObject & object)
{
    // Remove from object vector (O(1))
    if (object.index() > -1 && object.index() < static_cast<int>(objs.size()))
    {
        objs[object.index()] = objs.back();
        objs[object.index()]->setIndex(object.index());
        objs.pop_back();
        object.setIndex(-1);
    }
}

void MCWorld::restoreObjectToIntegration(MCObject & object)
{
    if (object.index() == -1)
    {
        // Add to object vector (O(1))
        objs.push_back(&object);
        object.setIndex(objs.size() - 1);
    }
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

void MCWorld::processCollisions()
{
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
        m_depthTestEnabled[layer] = enable;
    }
}

void MCWorld::addParticleVisibilityCamera(MCCamera & camera)
{
    m_visibilityCameras.push_back(&camera);
}

void MCWorld::removeParticleVisibilityCameras()
{
    m_visibilityCameras.clear();
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

void MCWorld::removeForceGenerators(MCObject & obj)
{
    forceRegistry.removeForceGenerators(obj);
}

void MCWorld::stepTime(MCFloat step)
{
    // Integrate physics
    integrate(step);

    // Process collisions and generate impulses
    processCollisions();

    // Remove objects that are marked to be removed
    processRemovedObjects();
}

MCWorld::ObjectVector MCWorld::objects() const
{
    return objs;
}

MCObjectTree & MCWorld::objectTree() const
{
    assert(m_objectTree);
    return *m_objectTree;
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
