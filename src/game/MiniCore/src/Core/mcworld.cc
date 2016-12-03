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
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mcrectshape.hh"
#include "mctrigonom.hh"
#include "mcworldrenderer.hh"

#include <cassert>

MCWorld * MCWorld::m_instance             = nullptr;
MCFloat   MCWorld::m_metersPerUnit        = 1.0;
MCFloat   MCWorld::m_metersPerUnitSquared = 1.0;

namespace {
const int REMOVED_INDEX = -1;
}

MCWorld::MCWorld()
: m_renderer(new MCWorldRenderer)
, m_forceRegistry(new MCForceRegistry)
, m_collisionDetector(new MCCollisionDetector)
, m_impulseGenerator(new MCImpulseGenerator)
, m_objectGrid(nullptr)
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
, m_resolverLoopCount(5)
, m_resolverStep(1.0 / m_resolverLoopCount)
, m_gravity(MCVector3dF(0, 0, -9.81))
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
    delete m_objectGrid;

    MCWorld::m_instance = nullptr;

    delete m_leftWallObject;
    delete m_rightWallObject;
    delete m_topWallObject;
    delete m_bottomWallObject;
}

void MCWorld::integrate(int step)
{
    // Integrate and update all registered objects
    m_forceRegistry->update();
    for (auto && object : m_objs)
    {
        if (object->isPhysicsObject() && !object->physicsComponent().isStationary())
        {
            object->physicsComponent().stepTime(step);
        }

        object->onStepTime(step);
    }
}

void MCWorld::detectCollisions()
{
    // Check collisions for all registered objects
    m_numCollisions = m_collisionDetector->detectCollisions(*m_objectGrid);
}

void MCWorld::generateImpulses()
{
    m_impulseGenerator->generateImpulsesFromDeepestContacts(m_objs);
}

void MCWorld::resolvePositions(MCFloat accuracy)
{
    m_impulseGenerator->resolvePositions(m_objs, accuracy);
}

void MCWorld::prepareRendering(MCCamera * camera)
{
    m_renderer->buildBatches(camera);
}

void MCWorld::render(MCCamera * camera)
{
    m_renderer->render(camera);
}

void MCWorld::renderShadows(MCCamera * camera)
{
    m_renderer->renderShadows(camera);
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
    for (MCObject * object : m_objs)
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
    m_objs.clear();
    m_removeObjs.clear();
}

void MCWorld::setDimensions(
    MCFloat minX, MCFloat maxX, MCFloat minY, MCFloat maxY, MCFloat minZ, MCFloat maxZ,
    MCFloat metersPerUnit, bool addAreaWalls, int gridSize)
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
    const MCFloat leafWidth = (maxX - minX) / gridSize;
    const MCFloat leafHeight = (maxY - minY) / gridSize;
    delete m_objectGrid;
    m_objectGrid = new MCObjectGrid(
        m_minX, m_minY,
        m_maxX, m_maxY,
        leafWidth, leafHeight);

    if (addAreaWalls)
    {
        // Create "wall" objects
        const MCFloat w = m_maxX - m_minX;
        const MCFloat h = m_maxY - m_minY;

        if (m_leftWallObject)
        {
            removeObjectNow(*m_leftWallObject);
            delete m_leftWallObject;
        }

        const MCFloat wallRestitution = 0.25f;

        m_leftWallObject = new MCObject("LEFT_WALL");
        m_leftWallObject->setShape(MCShapePtr(new MCRectShape(nullptr, w, h)));
        m_leftWallObject->physicsComponent().setMass(0, true);
        m_leftWallObject->physicsComponent().setRestitution(wallRestitution);
        m_leftWallObject->addToWorld();
        m_leftWallObject->translate(MCVector3dF(-w / 2, h / 2, 0));

        if (m_rightWallObject)
        {
            removeObjectNow(*m_rightWallObject);
            delete m_rightWallObject;
        }

        m_rightWallObject = new MCObject("RIGHT_WALL");
        m_rightWallObject->setShape(MCShapePtr(new MCRectShape(nullptr, w, h)));
        m_rightWallObject->physicsComponent().setMass(0, true);
        m_rightWallObject->physicsComponent().setRestitution(wallRestitution);
        m_rightWallObject->addToWorld();
        m_rightWallObject->translate(MCVector3dF(w + w / 2, h / 2, 0));

        if (m_topWallObject)
        {
            removeObjectNow(*m_topWallObject);
            delete m_topWallObject;
        }

        m_topWallObject = new MCObject("TOP_WALL");
        m_topWallObject->setShape(MCShapePtr(new MCRectShape(nullptr, w, h)));
        m_topWallObject->physicsComponent().setMass(0, true);
        m_topWallObject->physicsComponent().setRestitution(wallRestitution);
        m_topWallObject->addToWorld();
        m_topWallObject->translate(MCVector3dF(w / 2, h + h / 2, 0));

        if (m_bottomWallObject)
        {
            removeObjectNow(*m_bottomWallObject);
            delete m_bottomWallObject;
        }

        m_bottomWallObject = new MCObject("BOTTOM_WALL");
        m_bottomWallObject->setShape(MCShapePtr(new MCRectShape(nullptr, w, h)));
        m_bottomWallObject->physicsComponent().setMass(0, true);
        m_bottomWallObject->physicsComponent().setRestitution(wallRestitution);
        m_bottomWallObject->addToWorld();
        m_bottomWallObject->translate(MCVector3dF(w / 2, -h / 2, 0));
    }
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

int MCWorld::objectCount() const
{
    return static_cast<int>(m_objs.size());
}

void MCWorld::addObject(MCObject & object)
{
    if (!object.removing())
    {
        if (object.index() == REMOVED_INDEX)
        {
            m_renderer->add(object);

            // Add to object vector (O(1))
            m_objs.push_back(&object);
            object.setIndex(static_cast<int>(m_objs.size()) - 1);

            // Add to ObjectTree
            if ((object.isPhysicsObject() || object.isTriggerObject()) && !object.bypassCollisions())
            {
                m_objectGrid->insert(object);
            }

            // Add xy friction
            const MCFloat FrictionThreshold = 0.001f;
            if (object.physicsComponent().xyFriction() > FrictionThreshold)
            {
                m_forceRegistry->addForceGenerator(
                    MCForceGeneratorPtr(new MCFrictionGenerator(
                        object.physicsComponent().xyFriction(), object.physicsComponent().xyFriction())), object);
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
        for (MCObject * obj : m_objs)
        {
            if (obj != &object)
            {
                if (obj->isPhysicsObject())
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
    object.physicsComponent().reset();

    m_renderer->remove(object);

    // Remove from object vector (O(1))
    removeObjectFromIntegration(object);

    // Remove from ObjectTree
    if (object.isPhysicsObject() && !object.bypassCollisions())
    {
        m_objectGrid->remove(object);
    }

    object.setRemoving(false);
}

void MCWorld::removeObjectFromIntegration(MCObject & object)
{
    // Remove from object vector (O(1))
    if (object.index() > REMOVED_INDEX && object.index() < static_cast<int>(m_objs.size()))
    {
        m_objs[object.index()] = m_objs.back();
        m_objs[object.index()]->setIndex(object.index());
        m_objs.pop_back();
        object.setIndex(REMOVED_INDEX);
    }
}

void MCWorld::restoreObjectToIntegration(MCObject & object)
{
    if (object.index() == REMOVED_INDEX)
    {
        // Add to object vector (O(1))
        m_objs.push_back(&object);
        object.setIndex(static_cast<int>(m_objs.size()) - 1);
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
        generateImpulses();

        // Process contacts and generate impulses
        m_collisionDetector->enablePrimaryCollisionEvents(false);
        for (MCUint i = 0; i < m_resolverLoopCount && m_numCollisions > 0; i++)
        {
            detectCollisions();
            resolvePositions(m_resolverStep);
        }
        m_collisionDetector->enablePrimaryCollisionEvents(true);
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

MCWorld::ObjectVector MCWorld::objects() const
{
    return m_objs;
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

void MCWorld::setMetersPerUnit(MCFloat value)
{
    MCWorld::m_metersPerUnit        = value;
    MCWorld::m_metersPerUnitSquared = value * value;
}

MCFloat MCWorld::metersPerUnit()
{
    assert(MCWorld::m_instance);
    return MCWorld::m_metersPerUnit;
}

void MCWorld::toMeters(MCFloat & units)
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

void MCWorld::setResolverLoopCount(MCUint resolverLoopCount)
{
    m_resolverLoopCount = resolverLoopCount;
    m_resolverStep = 1.0f / resolverLoopCount;
}
