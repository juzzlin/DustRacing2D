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
#include "mcobjectgrid.hh"
#include "mcparticle.hh"
#include "mcphysicscomponent.hh"
#include "mcrectshape.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mctrigonom.hh"
#include "mcworldrenderer.hh"

#include <cassert>
#include <iostream>

MCWorld * MCWorld::m_instance = nullptr;

float MCWorld::m_metersPerUnit = 1.0;

float MCWorld::m_metersPerUnitSquared = 1.0;

namespace {
const int REMOVED_INDEX = -1;
}

MCWorld::MCWorld()
  : m_renderer(new MCWorldRenderer)
  , m_forceRegistry(new MCForceRegistry)
  , m_collisionDetector(new MCCollisionDetector)
  , m_impulseGenerator(new MCImpulseGenerator)
  , m_minX(0)
  , m_maxX(0)
  , m_minY(0)
  , m_maxY(0)
  , m_minZ(0)
  , m_maxZ(0)
  , m_numCollisions(0)
  , m_resolverLoopCount(5)
  , m_resolverStep(1.0f / m_resolverLoopCount)
  , m_gravity(MCVector3dF(0, 0, -9.81f))
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

    // Default dimensions. Creates also MCObjectGrid.
    setDimensions(0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0);
}

MCWorld::~MCWorld()
{
    clear();

    delete m_renderer;
    delete m_forceRegistry;
    delete m_collisionDetector;
    delete m_impulseGenerator;

    MCWorld::m_instance = nullptr;
}

void MCWorld::integrate(int step)
{
    // Integrate and update all registered objects
    m_forceRegistry->update();
    for (auto && object : m_objects)
    {
        if (object->isPhysicsObject() && !object->physicsComponent().isStationary())
        {
            object->physicsComponent().stepTime(step);
        }

        object->onStepTime(step);
    }
}

void MCWorld::generateImpulses()
{
    m_impulseGenerator->generateImpulsesFromDeepestContacts(m_objects);
}

void MCWorld::resolvePositions(float accuracy)
{
    m_impulseGenerator->resolvePositions(m_objects, accuracy);
}

void MCWorld::prepareRendering(MCCamera * camera)
{
    m_renderer->buildBatches(camera);
}

void MCWorld::render(MCCamera * camera, MCRenderGroup renderGroup)
{
    m_renderer->render(camera, renderGroup);
}

bool MCWorld::hasInstance()
{
    return MCWorld::m_instance;
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
    for (auto && object : m_objects)
    {
        object->deleteContacts();
        object->physicsComponent().reset();
        object->setIndex(REMOVED_INDEX);

        if (object->isParticle())
        {
            static_cast<MCParticle *>(object)->die();
        }
    }

    m_renderer->clear();
    m_objectGrid->removeAll();
    m_objects.clear();
    m_removeObjs.clear();
    m_collisionDetector->clear();
}

void MCWorld::setDimensions(
  float minX, float maxX, float minY, float maxY, float minZ, float maxZ,
  float metersPerUnit, bool addAreaWalls, size_t gridSize)
{
    assert(maxX - minX > 0);
    assert(maxY - minY > 0);
    assert(maxZ - minZ > 0);

    MCWorld::setMetersPerUnit(metersPerUnit);

    // Set dimensions
    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;
    m_minZ = minZ;
    m_maxZ = maxZ;

    // Init objectGrid
    const float leafWidth = (maxX - minX) / gridSize;
    const float leafHeight = (maxY - minY) / gridSize;

    m_objectGrid = std::make_unique<MCObjectGrid>(m_minX, m_minY, m_maxX, m_maxY, leafWidth, leafHeight);

    if (addAreaWalls)
    {
        // Create "wall" objects
        const float w = m_maxX - m_minX;
        const float h = m_maxY - m_minY;

        if (m_leftWallObject)
        {
            removeObjectNow(*m_leftWallObject);
        }

        const float wallRestitution = 0.25f;

        m_leftWallObject = std::make_unique<MCObject>("__WORLD_LEFT_WALL");
        m_leftWallObject->setShape(std::make_shared<MCRectShape>(nullptr, w, h));
        m_leftWallObject->physicsComponent().setMass(0, true);
        m_leftWallObject->physicsComponent().setRestitution(wallRestitution);
        m_leftWallObject->addToWorld();
        m_leftWallObject->translate(MCVector3dF(-w / 2, h / 2, 0));

        if (m_rightWallObject)
        {
            removeObjectNow(*m_rightWallObject);
        }

        m_rightWallObject = std::make_unique<MCObject>("__WORLD_RIGHT_WALL");
        m_rightWallObject->setShape(std::make_shared<MCRectShape>(nullptr, w, h));
        m_rightWallObject->physicsComponent().setMass(0, true);
        m_rightWallObject->physicsComponent().setRestitution(wallRestitution);
        m_rightWallObject->addToWorld();
        m_rightWallObject->translate(MCVector3dF(w + w / 2, h / 2, 0));

        if (m_topWallObject)
        {
            removeObjectNow(*m_topWallObject);
        }

        m_topWallObject = std::make_unique<MCObject>("__WORLD_TOP_WALL");
        m_topWallObject->setShape(std::make_shared<MCRectShape>(nullptr, w, h));
        m_topWallObject->physicsComponent().setMass(0, true);
        m_topWallObject->physicsComponent().setRestitution(wallRestitution);
        m_topWallObject->addToWorld();
        m_topWallObject->translate(MCVector3dF(w / 2, h + h / 2, 0));

        if (m_bottomWallObject)
        {
            removeObjectNow(*m_bottomWallObject);
        }

        m_bottomWallObject = std::make_unique<MCObject>("__WORLD_BOTTOM_WALL");
        m_bottomWallObject->setShape(std::make_shared<MCRectShape>(nullptr, w, h));
        m_bottomWallObject->physicsComponent().setMass(0, true);
        m_bottomWallObject->physicsComponent().setRestitution(wallRestitution);
        m_bottomWallObject->addToWorld();
        m_bottomWallObject->translate(MCVector3dF(w / 2, -h / 2, 0));
    }
    else
    {
        if (m_leftWallObject)
        {
            removeObjectNow(*m_leftWallObject);
            m_leftWallObject.reset();
        }

        if (m_rightWallObject)
        {
            removeObjectNow(*m_rightWallObject);
            m_rightWallObject.reset();
        }

        if (m_topWallObject)
        {
            removeObjectNow(*m_topWallObject);
            m_topWallObject.reset();
        }

        if (m_bottomWallObject)
        {
            removeObjectNow(*m_bottomWallObject);
            m_bottomWallObject.reset();
        }
    }
}

float MCWorld::minX() const
{
    return m_minX;
}

float MCWorld::maxX() const
{
    return m_maxX;
}

float MCWorld::minY() const
{
    return m_minY;
}

float MCWorld::maxY() const
{
    return m_maxY;
}

float MCWorld::minZ() const
{
    return m_minZ;
}

float MCWorld::maxZ() const
{
    return m_maxZ;
}

size_t MCWorld::objectCount() const
{
    return m_objects.size();
}

void MCWorld::addObject(MCObject & object)
{
    if (!object.removing())
    {
        if (object.index() == REMOVED_INDEX)
        {
            m_renderer->addObject(object);

            // Add to object vector (O(1))
            m_objects.push_back(&object);
            object.setIndex(static_cast<int>(m_objects.size()) - 1);

            m_objectGrid->insert(object);

            // Add xy friction
            const float FrictionThreshold = 0.001f;
            if (object.physicsComponent().xyFriction() > FrictionThreshold)
            {
                m_forceRegistry->addForceGenerator(
                  std::make_shared<MCFrictionGenerator>(
                    object.physicsComponent().xyFriction(), object.physicsComponent().xyFriction()),
                  object);
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
    if (object.index() > REMOVED_INDEX || object.physicsComponent().isSleeping())
    {
        object.setRemoving(true);
        m_removeObjs.push_back(&object);
    }
}

void MCWorld::removeObjectNow(MCObject & object)
{
    if (object.index() > REMOVED_INDEX || object.physicsComponent().isSleeping())
    {
        object.setRemoving(true);
        for (auto && obj : m_objects)
        {
            if (obj != &object && obj->isPhysicsObject())
            {
                obj->deleteContacts(object);
            }
        }

        doRemoveObject(object);
    }
}

void MCWorld::doRemoveObject(MCObject & object)
{
    // Reset motion
    object.physicsComponent().reset();

    m_renderer->removeObject(object);

    // Remove from object vector (O(1))
    removeObjectFromIntegration(object);

    // Remove from ObjectTree
    if (object.isPhysicsObject() && !object.bypassCollisions())
    {
        m_objectGrid->remove(object);
        m_collisionDetector->remove(object);
    }

    object.setRemoving(false);
}

void MCWorld::removeObjectFromIntegration(MCObject & object)
{
    // Remove from object vector (O(1))
    if (object.index() > REMOVED_INDEX && object.index() < static_cast<int>(m_objects.size()))
    {
        m_objects[static_cast<size_t>(object.index())] = m_objects.back();
        m_objects[static_cast<size_t>(object.index())]->setIndex(object.index());
        m_objects.pop_back();
        object.setIndex(REMOVED_INDEX);
    }
}

void MCWorld::restoreObjectToIntegration(MCObject & object)
{
    if (object.index() == REMOVED_INDEX)
    {
        // Add to object vector (O(1))
        m_objects.push_back(&object);
        object.setIndex(static_cast<int>(m_objects.size()) - 1);
    }
}

void MCWorld::processRemovedObjects()
{
    for (auto && obj : m_removeObjs)
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
    // Check collisions for all registered objects
    m_numCollisions = m_collisionDetector->detectCollisions(*m_objectGrid);
    if (m_numCollisions)
    {
        generateImpulses();

        // Process contacts and generate impulses
        for (size_t i = 0; i < m_resolverLoopCount && m_numCollisions > 0; i++)
        {
            m_numCollisions = m_collisionDetector->iterateCurrentCollisions();
            resolvePositions(m_resolverStep);
        }
    }
}

MCForceRegistry & MCWorld::forceRegistry() const
{
    assert(m_forceRegistry);
    return *m_forceRegistry;
}

void MCWorld::stepTime(int step)
{
    // Integrate physics
    integrate(step);

    // Process collisions and generate impulses
    processCollisions();

    // Remove objects that are marked to be removed
    processRemovedObjects();
}

const MCWorld::ObjectVector & MCWorld::objects() const
{
    return m_objects;
}

MCObjectGrid & MCWorld::objectGrid() const
{
    assert(m_objectGrid);
    return *m_objectGrid;
}

MCWorldRenderer & MCWorld::renderer() const
{
    assert(m_renderer);
    return *m_renderer;
}

void MCWorld::setGravity(const MCVector3dF & gravity)
{
    m_gravity = gravity;
}

const MCVector3dF & MCWorld::gravity() const
{
    return m_gravity;
}

void MCWorld::setMetersPerUnit(float value)
{
    MCWorld::m_metersPerUnit = value;
    MCWorld::m_metersPerUnitSquared = value * value;
}

float MCWorld::metersPerUnit()
{
    assert(MCWorld::m_instance);
    return MCWorld::m_metersPerUnit;
}

void MCWorld::toMeters(float & units)
{
    units *= MCWorld::m_metersPerUnit;
}

void MCWorld::toMeters(MCVector2dF & units)
{
    units *= MCWorld::m_metersPerUnit;
}

void MCWorld::toMeters(MCVector3dF & units)
{
    units *= MCWorld::m_metersPerUnit;
}

void MCWorld::setResolverLoopCount(size_t resolverLoopCount)
{
    m_resolverLoopCount = resolverLoopCount;
    m_resolverStep = 1.0f / resolverLoopCount;
}
