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
#include "mcforceregistry.hh"
#include "mcforcegenerator.hh"
#include "mccontactresolver.hh"
#include "mcobject.hh"
#include "mcquadtree.hh"
#include "mccamera.hh"

MCWorld * MCWorldImpl::m_pInstance = nullptr;

MCWorldImpl::MCWorldImpl()
: m_pForceRegistry(new MCForceRegistry)
, m_pResolver(new MCContactResolver)
, minX(0)
, maxX(0)
, minY(0)
, maxY(0)
, minZ(0)
, maxZ(0)
{}

MCWorldImpl::~MCWorldImpl()
{}

void MCWorldImpl::integrate(MCFloat step)
{
    // Integrate and update all registered objects
    m_pForceRegistry->update();
    for (UINT i = 0; i < m_objs.size(); i++) {
        MCObject * const p(m_objs[i]);
        if (p->physicsObject() && !p->stationary()) {
            p->integrate(step);
            p->stepTime();
        } else {
            p->stepTime();
        }
    }
}

void MCWorldImpl::detectCollisions()
{
    // Check collisions for all registered objects
    static MCQuadtree::ObjectSet collisions;
    MCQuadtree::ObjectSet::iterator j;
    MCQuadtree::ObjectSet::iterator j2;
    for (UINT i = 0; i < m_objs.size(); i++) {
        MCObject * const p(m_objs[i]);
        if (p->physicsObject()) {
            collisions.clear();
            m_pQuadtree->getBBoxCollisions(p, collisions);
            j  = collisions.begin();
            j2 = collisions.end();
            while (j != j2) {
                m_pResolver->processPossibleCollision(p, *j);
                j++;
            }
        }
    }
}

MCContact * MCWorldImpl::getDeepestInterpenetration(
    const std::vector<MCContact *> & contacts)
{
    MCFloat maxDepth = 0;
    MCContact * bestContact = nullptr;
    Q_FOREACH(MCContact * contact, contacts) {
        if (contact->interpenetrationDepth() > maxDepth) {
            maxDepth = contact->interpenetrationDepth();
            bestContact = contact;
        }
    }
    return bestContact;
}

void MCWorldImpl::processContacts(MCObject * p)
{
    MCContact * deepestContact = nullptr;
    MCObject::ContactHash::const_iterator iter(p->contacts().begin());
    iter = p->contacts().begin();
    for (; iter != p->contacts().end(); iter++) {
        deepestContact = getDeepestInterpenetration(iter->second);
        if (deepestContact) {
            processContact(p, deepestContact);
        }
    }
    p->deleteContacts();
}

void MCWorldImpl::processContacts()
{
    Q_FOREACH(MCObject * obj, m_objs) {
        if (obj->physicsObject()) {
            processContacts(obj);
        }
    }
}

void MCWorldImpl::processContact(MCObject * pObject, MCContact * pContact)
{
    MCObject * const pa(pObject);
    MCObject * const pb(pContact->object());

    const MCFloat restitution(std::min(pObject->restitution(), pContact->object()->restitution()));
    const MCVector2d<MCFloat> diff(pb->velocity() - pa->velocity());
    const MCVector3d<MCFloat> impulse(pContact->contactNormal() * pContact->contactNormal().dot(diff));
    const MCVector3d<MCFloat> displacement(pContact->contactNormal() * pContact->interpenetrationDepth());

    const MCFloat a = pa->invMass();
    const MCFloat b = pb->invMass();

    if (!pa->stationary()) {
        // Move colliding object
        const MCFloat s = a / (a + b);
        pa->displace(displacement * s);

        // Apply impulse
        pa->addImpulse((impulse + impulse * restitution) * s);
    }

    if (!pb->stationary()) {
        // Move colliding object
        const MCFloat s = b / (a + b);
        pb->displace(-displacement * s);

        // Apply impulse
        pb->addImpulse((-impulse - impulse * restitution) * s);
    }

    // Remove contacts with pa from pb, because physically it was already handled here
    pb->deleteContacts(pa);
}

void MCWorldImpl::render(MCCamera * pCamera)
{
    // Render in the order of the layers
    LayerHash::iterator j;
    LayerHash::iterator end;
    MCObject * pObj = nullptr;
    for (UINT i = 0; i < MCWorld::MAX_LAYERS; i++) {
        j   = m_layers[i].begin();
        end = m_layers[i].end();
        for (; j != end; j++) {
            pObj = *j;
            if (pObj->renderable()) {
                if ((pCamera && pCamera->isVisible(pObj->bbox())) || !pCamera) {
                    pObj->render(pCamera);
                }
            }
        }
    }
}

void MCWorldImpl::renderShadows(MCCamera * pCamera)
{
    const UINT i2 = m_objs.size();
    MCObject * pObj = nullptr;
    for (UINT i = 0; i < i2; i++) {
        pObj = m_objs[i];
        if (pObj->renderable() && pObj->hasShadow()) {
            if ((pCamera && pCamera->isVisible(pObj->bbox())) || !pCamera) {
                pObj->renderShadow(pCamera);
            }
        }
    }
}

MCWorld::MCWorld() :
    m_pImpl(new MCWorldImpl)    
{
    if (!MCWorldImpl::m_pInstance) {
        MCWorldImpl::m_pInstance = this;
    } else {
        std::cerr << "ERROR!!: Only one MCWorld can exist!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

MCWorld * MCWorld::instance()
{
    if (!MCWorldImpl::m_pInstance) {
        std::cerr << "ERROR!!: MCWorld instance not created!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return MCWorldImpl::m_pInstance;
}

void MCWorld::setDimensions(MCFloat minX_, MCFloat maxX_, MCFloat minY_, MCFloat maxY_,
                            MCFloat minZ_, MCFloat maxZ_)
{
    // Set dimensions for minimum quadtree leaves
    const MCFloat MIN_LEAF_WIDTH  = 64;
    const MCFloat MIN_LEAF_HEIGHT = 64;

    // Init quadtree
    m_pImpl->m_pQuadtree = new MCQuadtree(minX_, minY_, maxX_, maxY_,
                                          MIN_LEAF_WIDTH, MIN_LEAF_HEIGHT);
    // Set dimensions
    m_pImpl->minX = minX_;
    m_pImpl->maxX = maxX_;
    m_pImpl->minY = minY_;
    m_pImpl->maxY = maxY_;
    m_pImpl->minZ = minZ_;
    m_pImpl->maxZ = maxZ_;
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

void MCWorld::addObject(MCObject * p)
{
    m_pImpl->addObject(p);
}

void MCWorldImpl::addObject(MCObject * p)
{
    if (!p->removing()) {
        if (p->index() == -1) {

            // Add to layer map
            addToLayerMap(p);

            // Add to object vector (O(1))
            m_objs.push_back(p);
            p->setIndex(m_objs.size() - 1);

            // Add to Quadtree
            if (p->physicsObject() && !p->bypassCollisions()) {
                m_pQuadtree->insert(p);
            }
        }
    } else {
        p->setRemoving(false);
    }
}

void MCWorld::removeObject(MCObject * p)
{
    p->setRemoving(true);
    m_pImpl->m_removeObjs.push_back(p);
}

void MCWorld::removeObjectNow(MCObject * p)
{
    p->setRemoving(true);
    Q_FOREACH(MCObject * obj, m_pImpl->m_objs) {
        if (obj->physicsObject()) {
            obj->deleteContacts(p);
        }
    }

    m_pImpl->removeObject(p);
}

void MCWorldImpl::removeObject(MCObject * p)
{
    // Reset motion
    p->resetMotion();

    // Remove from the layer map
    removeFromLayerMap(p);

    // Remove from object vector (O(1))
    if (p->index() > -1 && p->index() < static_cast<int>(m_objs.size())) {
        m_objs[p->index()] = m_objs.back();
        m_objs[p->index()]->setIndex(p->index());
        m_objs.pop_back();
        p->setIndex(-1);
    }

    // Remove from Quadtree
    if (p->physicsObject() && !p->bypassCollisions()) {
        m_pQuadtree->remove(p);
    }

    p->setRemoving(false);
}

void MCWorldImpl::removeObjects()
{
    Q_FOREACH(MCObject * obj, m_removeObjs) {
        if (obj->removing()) {
            removeObject(obj);
        }
    }
    m_removeObjs.clear();
}

void MCWorldImpl::addToLayerMap(MCObject * p)
{
    const UINT layerIndex = p->layer() >= MCWorld::MAX_LAYERS ? MCWorld::MAX_LAYERS - 1 : p->layer();
    m_layers[layerIndex].insert(p);
}

void MCWorldImpl::removeFromLayerMap(MCObject * p)
{
    const UINT layerIndex = p->layer() >= MCWorld::MAX_LAYERS ? MCWorld::MAX_LAYERS - 1 : p->layer();
    m_layers[layerIndex].erase(p);
}

void MCWorld::addToLayerMap(MCObject * p)
{
    m_pImpl->addToLayerMap(p);
}

void MCWorld::removeFromLayerMap(MCObject * p)
{
    m_pImpl->removeFromLayerMap(p);
}

void MCWorld::addForceGenerator(MCForceGenerator * pGen, MCObject * pObj, bool takeOwnership)
{
    m_pImpl->m_pForceRegistry->addForceGenerator(pGen, pObj, takeOwnership);
}

void MCWorld::removeForceGenerator(MCForceGenerator * pGen, MCObject * pObj)
{
    m_pImpl->m_pForceRegistry->removeForceGenerator(pGen, pObj);
}

void MCWorld::stepTime(MCFloat step)
{
    // Integrate physics
    m_pImpl->integrate(step);

    // Detect all collisions and generate contacts
    m_pImpl->detectCollisions();

    // Process contacts
    m_pImpl->processContacts();

    // Remove objects that are marked to be removed
    m_pImpl->removeObjects();
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
    return m_pImpl->m_objs;
}

MCQuadtree * MCWorld::tree() const
{
    return m_pImpl->m_pQuadtree;
}

MCWorld::~MCWorld()
{
    delete m_pImpl->m_pForceRegistry;
    delete m_pImpl->m_pResolver;
    delete m_pImpl->m_pQuadtree;
}
