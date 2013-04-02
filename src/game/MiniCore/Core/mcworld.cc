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
#include "mccamera.hh"
#include "mccollisiondetector.hh"
#include "mcforcegenerator.hh"
#include "mcforceregistry.hh"
#include "mcfrictiongenerator.hh"
#include "mcimpulsegenerator.hh"
#include "mcmathutil.hh"
#include "mcobject.hh"
#include "mcobjecttree.hh"
#include "mcparticle.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mcrectshape.hh"
#include "mctrigonom.hh"
#include "mcworldrenderer.hh"

#include <cassert>

MCWorld * MCWorld::m_instance              = nullptr;
MCFloat   MCWorld::m_metersPerPixel        = 1.0;
MCFloat   MCWorld::m_metersPerPixelSquared = 1.0;

namespace
{
// Set dimensions for minimum objectTree leaves
const MCFloat MinLeafWidth  = 64;
const MCFloat MinLeafHeight = 64;
}

MCWorld::MCWorld()
: m_renderer(new MCWorldRenderer)
, m_forceRegistry(new MCForceRegistry)
, m_collisionDetector(new MCCollisionDetector)
, m_impulseGenerator(new MCImpulseGenerator)
, m_objectTree(nullptr)
, m_minX(0)
, m_maxX(0)
, m_minY(0)
, m_maxY(0)
, m_minZ(0)
, m_maxZ(0)
, m_leftWallObject(nullptr)
, m_rightWallObject(nullptr)
, m_topWallObject(nullptr)
, m_bottomWallObject(nullptr)
, m_numCollisions(0)
, m_numResolverLoops(5)
, m_resolverStep(1.0 / m_numResolverLoops)
{
    if (!MCWorld::m_instance)
    {
        MCWorld::m_instance = this;
    }
    else
    {
        std::cerr << "ERROR!!: Only one MCWorld can exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Default dimensions. Creates also MCObjectTree.
    setDimensions(0.0, MinLeafWidth, 0.0, MinLeafHeight, 0.0, 1.0, 1.0);
}

MCWorld::~MCWorld()
{
    clear();

    delete m_renderer;
    delete m_forceRegistry;
    delete m_collisionDetector;
    delete m_impulseGenerator;
    delete m_objectTree;
    delete m_leftWallObject;
    delete m_rightWallObject;
    delete m_topWallObject;
    delete m_bottomWallObject;

    MCWorld::m_instance = nullptr;
}

void MCWorld::integrate(MCFloat step)
{
    // Integrate and update all registered objects
    m_forceRegistry->update();
    const MCUint objectCount = m_objs.size();
    for (MCUint i = 0; i < objectCount; i++)
    {
        MCObject & object(*m_objs[i]);
        if (object.physicsObject() && !object.stationary())
        {
            object.integrate(step);
        }

        object.stepTime(step);
    }
}

void MCWorld::detectCollisions()
{
    // Check collisions for all registered objects
    m_numCollisions = m_collisionDetector->detectCollisions(*m_objectTree);
}

void MCWorld::generateImpulses()
{
    m_impulseGenerator->generateImpulsesFromDeepestContacts(m_objs);
}

void MCWorld::resolvePositions(MCFloat accuracy)
{
    m_impulseGenerator->resolvePositions(m_objs, accuracy);
}

void MCWorld::render(MCCamera * camera, bool enableShadows)
{
    m_renderer->render(camera, enableShadows);
}

MCWorld & MCWorld::instance()
{
    if (!MCWorld::m_instance)
    {
        std::cerr << "ERROR!!: MCWorld instance not created!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return *MCWorld::m_instance;
}

void MCWorld::clear()
{
    // This does the same as removeObject(), but the removal
    // process here is simpler as all data structures will be
    // cleared and all objects will be removed at once.
    for (MCObject * object : m_objs)
    {
        object->deleteContacts();
        object->resetMotion();
        object->setIndex(-1);

        if (object->isParticle())
        {
            static_cast<MCParticle *>(object)->die();
        }
    }

    m_renderer->clear();
    m_objectTree->removeAll();
    m_objs.clear();
    m_removeObjs.clear();
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

    if (m_leftWallObject)
    {
        removeObjectNow(*m_leftWallObject);
        delete m_leftWallObject;
    }

    m_leftWallObject = new MCObject("LEFT_WALL");
    m_leftWallObject->setShape(new MCRectShape(nullptr, w, h));
    m_leftWallObject->setMass(0, true);
    m_leftWallObject->setRestitution(0.25);
    m_leftWallObject->addToWorld();
    m_leftWallObject->translate(MCVector3dF(-w / 2, h / 2, 0));

    if (m_rightWallObject)
    {
        removeObjectNow(*m_rightWallObject);
        delete m_rightWallObject;
    }

    m_rightWallObject = new MCObject("RIGHT_WALL");
    m_rightWallObject->setShape(new MCRectShape(nullptr, w, h));
    m_rightWallObject->setMass(0, true);
    m_rightWallObject->setRestitution(0.25);
    m_rightWallObject->addToWorld();
    m_rightWallObject->translate(MCVector3dF(w + w / 2, h / 2, 0));

    if (m_topWallObject)
    {
        removeObjectNow(*m_topWallObject);
        delete m_topWallObject;
    }

    m_topWallObject = new MCObject("TOP_WALL");
    m_topWallObject->setShape(new MCRectShape(nullptr, w, h));
    m_topWallObject->setMass(0, true);
    m_topWallObject->setRestitution(0.25);
    m_topWallObject->addToWorld();
    m_topWallObject->translate(MCVector3dF(w / 2, h + h / 2, 0));

    if (m_bottomWallObject)
    {
        removeObjectNow(*m_bottomWallObject);
        delete m_bottomWallObject;
    }

    m_bottomWallObject = new MCObject("BOTTOM_WALL");
    m_bottomWallObject->setShape(new MCRectShape(nullptr, w, h));
    m_bottomWallObject->setMass(0, true);
    m_bottomWallObject->setRestitution(0.25);
    m_bottomWallObject->addToWorld();
    m_bottomWallObject->translate(MCVector3dF(w / 2, -h / 2, 0));
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
            m_renderer->addToLayerMap(object);

            // Add to object vector (O(1))
            m_objs.push_back(&object);
            object.setIndex(m_objs.size() - 1);

            // Add to ObjectTree
            if (object.physicsObject() && !object.bypassCollisions())
            {
                m_objectTree->insert(object);
            }

            // Add xy friction
            const MCFloat FrictionThreshold = 0.001;
            if (object.xyFriction() > FrictionThreshold)
            {
                m_forceRegistry->addForceGenerator(
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
        m_removeObjs.push_back(&object);
    }
}

void MCWorld::removeObjectNow(MCObject & object)
{
    if (object.index() >= 0)
    {
        object.setRemoving(true);
        for (MCObject * obj : m_objs)
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
    m_renderer->removeFromLayerMap(object);

    // Remove from object vector (O(1))
    if (object.index() > -1 && object.index() < static_cast<int>(m_objs.size()))
    {
        m_objs[object.index()] = m_objs.back();
        m_objs[object.index()]->setIndex(object.index());
        m_objs.pop_back();
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
    if (object.index() > -1 && object.index() < static_cast<int>(m_objs.size()))
    {
        m_objs[object.index()] = m_objs.back();
        m_objs[object.index()]->setIndex(object.index());
        m_objs.pop_back();
        object.setIndex(-1);
    }
}

void MCWorld::restoreObjectToIntegration(MCObject & object)
{
    if (object.index() == -1)
    {
        // Add to object vector (O(1))
        m_objs.push_back(&object);
        object.setIndex(m_objs.size() - 1);
    }
}

void MCWorld::processRemovedObjects()
{
    for (MCObject * obj : m_removeObjs)
    {
        if (obj->removing())
        {
            doRemoveObject(*obj);
        }
    }

    m_removeObjs.clear();
}

void MCWorld::processCollisions()
{
    detectCollisions();

    if (m_numCollisions)
    {
        for (MCUint i = 0; i < m_numResolverLoops; i++)
        {
            generateImpulses();
        }

        // Process contacts and generate impulses
        m_collisionDetector->enableCollisionEvents(false);
        for (MCUint i = 0; i < m_numResolverLoops; i++)
        {
            detectCollisions();
            resolvePositions(m_resolverStep);
        }
        m_collisionDetector->enableCollisionEvents(true);
    }
}

void MCWorld::addForceGenerator(
    MCForceGenerator & gen, MCObject & obj, bool takeOwnership)
{
    m_forceRegistry->addForceGenerator(gen, obj, takeOwnership);
}

void MCWorld::removeForceGenerator(MCForceGenerator & gen, MCObject & obj)
{
    m_forceRegistry->removeForceGenerator(gen, obj);
}

void MCWorld::removeForceGenerators(MCObject & obj)
{
    m_forceRegistry->removeForceGenerators(obj);
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
    return m_objs;
}

MCObjectTree & MCWorld::objectTree() const
{
    assert(m_objectTree);
    return *m_objectTree;
}

MCWorldRenderer & MCWorld::renderer() const
{
    assert(m_renderer);
    return *m_renderer;
}

void MCWorld::setMetersPerPixel(MCFloat value)
{
    MCWorld::m_metersPerPixel        = value;
    MCWorld::m_metersPerPixelSquared = value * value;
}

MCFloat MCWorld::metersPerPixel()
{
    assert(MCWorld::m_instance);
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
