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

#ifndef MCWORLD_HH
#define MCWORLD_HH

#include <vector>
#include <unordered_set>

#include "mcvector2d.hh"
#include "mcvector3d.hh"
#include "mcmacros.hh"
#include "mctypes.hh"
#include "Physics/mcforceregistry.hh"
#include "Physics/mccollisiondetector.hh"
#include "Physics/mcimpulsegenerator.hh"

class MCObjectTree;
class MCForceGenerator;
class MCCamera;
class MCContact;
class MCObject;

//! \class World base class
//! \brief World class holds every MCObject in the scene
//!
//! MCWorld class holds every MCObject -derived object in the scene.
//! It applies the physics integration and collision detections for
//! the objects.
//!
//! MCWorld has always positive Z-axis pointing "up" and objects
//! move on the XY-plane. Direction of the gravity can be freely set.
class MCWorld
{
public:

    typedef std::vector<MCObject *> ObjectVector;

    //! Constructor.
    MCWorld();

    //! Destructor.
    virtual ~MCWorld();

    //! Return the one-and-only MCWorld instance
    static MCWorld & instance();

    //! Set dimensions of the world box.
    void setDimensions(
        MCFloat minX, MCFloat maxX, MCFloat minY, MCFloat maxY, MCFloat minZ, MCFloat maxZ,
        MCFloat metersPerPixel);

    //! Set how many meters equal one pixel.
    static void setMetersPerPixel(MCFloat value);

    //! Get how many meters equal one pixel.
    static MCFloat metersPerPixel();

    //! Convert pixels to meters.
    static void toMeters(MCFloat & pixels);

    //! Convert pixels to meters.
    static void toMeters(MCVector2dF & pixels);

    //! Convert pixels to meters.
    static void toMeters(MCVector3dF & pixels);

    //! Add object to the world. Object's current location is used.
    //! \param object Object to be added.
    void addObject(MCObject & object);

    //! Schedules the object to be removed from the world. This method
    //! should be used when removing objects in the middle of a run.
    //! Object is not removed immediately so possibly on-going physics
    //! and collision calculations are not messed up. Do not delete the object.
    //! \param object Object to be removed.
    void removeObject(MCObject & object);

    //! Removes object from the world immediately. This can be used if the object
    //! gets deleted. Slow.
    //! \param object Object to be removed.
    void removeObjectNow(MCObject & object);

    //! \brief Add force generator to an object.
    //! \param generator Generator to be added.
    //! \param object Target object the force applies to.
    //! \param takeOwnership If true, MCWorld takes the Ownership
    //!        and handles deletion of generator.
    void addForceGenerator(
        MCForceGenerator & generator,
        MCObject & object, bool takeOwnership = false);

    //! \brief Remove force generator assigned to an object.
    //! \param generator Generator to be added.
    //! \param object Target object the force applies to.
    void removeForceGenerator(
        MCForceGenerator & generator, MCObject & object);

    //! \brief Step world time
    //! This causes the integration of physics and executes
    //! collision detections.
    //! \param step Time step to be updated
    void stepTime(MCFloat step);

    //! \brief Render all registered objects.
    //! \param pCamera The camera window to be used. If nullptr, then
    //!        no any translations or clipping done.
    virtual void render(MCCamera * pCamera = nullptr);

    //! \brief Render fake shadows of all registered objects.
    //! \param pCamera The camera window to be used. If nullptr, then
    //!        no any translations or clipping done.
    virtual void renderShadows(MCCamera * pCamera = nullptr);

    //! \brief Return reference to the object objectTree.
    //! \return Pointer to the objectTree.
    MCObjectTree & objectTree() const;

    //! Get minimum X
    MCFloat minX() const;

    //! Get maximum X
    MCFloat maxX() const;

    //! Get minimum Y
    MCFloat minY() const;

    //! Get maximum Y
    MCFloat maxY() const;

    //! Get minimum Z
    MCFloat minZ() const;

    //! Get maximum Z
    MCFloat maxZ() const;

    //! Add p to the internal layer map. Can be used if a layer change is needed.
    void addToLayerMap(MCObject & object);

    //! Remove p from the internal layer map. Can be used if a layer change is needed.
    void removeFromLayerMap(MCObject & object);

    //! Enable/disable depth test on a layer.
    void enableDepthTestOnLayer(MCUint layer, bool enable);

    //! Max number of rendering layers
    static const MCUint MaxLayers = 32;

protected:

    //! Get registered objects
    ObjectVector objects() const;

private:

    DISABLE_COPY(MCWorld);
    DISABLE_ASSI(MCWorld);

    void integrate(MCFloat step);
    void processRemovedObjects();
    void doRemoveObject(MCObject & object);
    void detectCollisions();
    void generateImpulses();
    void resolvePositions(MCFloat accuracy);
    MCContact * getDeepestInterpenetration(const std::vector<MCContact *> & contacts);
    static MCWorld * pInstance;
    MCForceRegistry forceRegistry;
    MCCollisionDetector collisionDetector;
    MCImpulseGenerator impulseGenerator;
    MCObjectTree * pObjectTree;
    MCFloat m_minX, m_maxX, m_minY, m_maxY, m_minZ, m_maxZ;
    static MCFloat m_metersPerPixel;
    static MCFloat m_metersPerPixelSquared;
    typedef std::unordered_set<MCObject *> LayerHash;
    LayerHash layers[MCWorld::MaxLayers];
    bool depthTestEnabled[MCWorld::MaxLayers];
    MCWorld::ObjectVector objs;
    MCWorld::ObjectVector removeObjs;
    MCWorld::ObjectVector collidingObjs;
    MCObject * pLeft;
    MCObject * pRight;
    MCObject * pTop;
    MCObject * pBottom;
    MCUint numCollisions;
    MCUint numResolverLoops;
    MCFloat resolverStep;
};

#endif // MCWORLD_HH
