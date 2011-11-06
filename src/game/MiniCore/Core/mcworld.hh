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

#include <memory>
#include <vector>

#include "mcvector3d.hh"
#include "mcmacros.hh"
#include "mctypes.hh"

class MCWorldImpl;
class MCQuadtree;
class MCForceGenerator;
class MCCamera;
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
    static MCWorld * instance();

    //! Set dimensions of the world box
    void setDimensions(MCFloat minX, MCFloat maxX, MCFloat minY, MCFloat maxY,
                       MCFloat minZ, MCFloat maxZ);

    //! Add object to the world. Object's current location is used.
    //! \param p Object to be added.
    void addObject(MCObject * p);

    //! Schedules the object to be removed from the world. This method
    //! should be used when removing objects in the middle of a run.
    //! Object is not removed immediately so possibly on-going physics
    //! and collision calculations are not messed up. Do not delete the object.
    //! \param p Object to be removed.
    void removeObject(MCObject * p);

    //! Removes object from the world immediately. This can be used if the object
    //! gets deleted. Slow.
    //! \param p Object to be removed.
    void removeObjectNow(MCObject * p);

    //! \brief Add force generator to an object.
    //! \param pGen Generator to be added.
    //! \param pObj Target object.
    //! \param takeOwnership If true, MCWorld takes the Ownership
    //!        and handles deletion of generator.
    void addForceGenerator(MCForceGenerator * pGen, MCObject * pObj, bool takeOwnership = false);

    //! \brief Remove force generator assigned to an object.
    //! \param pGen Generator to be added.
    //! \param pObj Target object.
    void removeForceGenerator(MCForceGenerator * pGen, MCObject * pObj);

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

    //! \brief Return pointer to the object quadtree.
    //! \return Pointer to the quadtree.
    MCQuadtree * tree() const;

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
    void addToLayerMap(MCObject * p);

    //! Remove p from the internal layer map. Can be used if a layer change is needed.
    void removeFromLayerMap(MCObject * p);

    //! Max number of rendering layers
    static const MCUint MAX_LAYERS = 32;

protected:

    //! Get registered objects
    ObjectVector objects() const;

private:

    DISABLE_COPY(MCWorld);
    DISABLE_ASSI(MCWorld);
    std::shared_ptr<MCWorldImpl> const m_pImpl;
    friend class MCWorldImpl;
};

#endif // MCWORLD_HH
