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

#include "mcmacros.hh"
#include "mctypes.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <vector>

class MCCamera;
class MCCollisionDetector;
class MCContact;
class MCForceRegistry;
class MCImpulseGenerator;
class MCObject;
class MCObjectGrid;
class MCWorldRenderer;

/*! \class World base class.
 *  \brief World class holds every MCObject in the scene.
 *
 * MCWorld class holds every MCObject-derived object in the scene.
 * It applies the physics integration and collision detections for
 * the objects.
 *
 * MCWorld has always positive Z-axis pointing "up" and objects
 * move on the XY-plane. Direction of the gravity can be freely set.
 *
 * MCWorld uses MCWorldRenderer to render the scene.
 */
class MCWorld
{
public:

    typedef std::vector<MCObject *> ObjectVector;

    //! Constructor.
    MCWorld();

    //! Destructor.
    virtual ~MCWorld();

    //! Return the one-and-only MCWorld instance.
    static MCWorld & instance();

    static bool hasInstance();

    //! Remove all objects.
    void clear();

    /*! Set dimensions of the world box in units.
     *
     *  \param metersPerUnit You could have, for example, a game object that is a
     *  MCSurface of 10x20 units, but want it to behave like an real-world object
     *  of 1x2 meters, so the metersPerUnits would be 0.1 in that case.
     *
     *  \param gridSize ver and hor size of the object grid. This affects the collision
     *  detection performance.
     */
    void setDimensions(
        MCFloat minX,
        MCFloat maxX,
        MCFloat minY,
        MCFloat maxY,
        MCFloat minZ,
        MCFloat maxZ,
        MCFloat metersPerUnit = 1.0f,
        bool addAreaWalls = true,
        int gridSize = 128);

    /*! Set gravity vector used by default friction generators (on XY-plane).
     *  The default is [0, 0, -9.81]. Set the gravity (acceleration) for objects
     *  independently when needed. */
    void setGravity(const MCVector3dF & gravity);

    const MCVector3dF & gravity() const;

    //! Set how many meters equal one unit in the scene.
    static void setMetersPerUnit(MCFloat value);

    //! Get how many meters equal one unit in the scene.
    static MCFloat metersPerUnit();

    //! Convert scene units to meters.
    static void toMeters(MCFloat & units);

    //! Convert scene units to meters.
    static void toMeters(MCVector2dF & units);

    //! Convert scene units to meters.
    static void toMeters(MCVector3dF & units);

    /*! Add object to the world. Object's current location is used.
     *  \param object Object to be added. */
    void addObject(MCObject & object);

    /*! Schedules the object to be removed from the world. This method
     *  should be used when removing objects in the middle of a run.
     *  Object is not removed immediately so possibly on-going physics
     *  and collision calculations are not messed up. Do not delete the object.
     *  \param object Object to be removed. */
    void removeObject(MCObject & object);

    /*! Removes object from the world immediately. This can be used if the object
     *  gets deleted. Slow.
     *  \param object Object to be removed. */
    void removeObjectNow(MCObject & object);

    //! Stop integrating the given object.
    void removeObjectFromIntegration(MCObject & object);

    //! Restart integrating the given object.
    void restoreObjectToIntegration(MCObject & object);

    //! \return Force registry. Use this to add force generators to objects.
    MCForceRegistry & forceRegistry() const;

    /*! \brief Step world time
     *  This causes the integration of physics and executes collision detections.
     *  \param step Time step to be updated in msecs. */
    void stepTime(int step);

    /*! \brief Call this (once) before calling render() or renderShadows().
     *  \param camera The camera window to be used. If nullptr, then
     *         no any translations or clipping done. */
    virtual void prepareRendering(MCCamera * camera);

    /*! \brief Render all registered objects.
     *  \param camera Camera box, can be nullptr. */
    virtual void render(MCCamera * camera);

    /*! \brief Render shadows of all registered objects.
     *  \param camera Camera box, can be nullptr. */
    virtual void renderShadows(MCCamera * camera);

    //! \return Reference to the objectGrid.
    MCObjectGrid & objectGrid() const;

    //! \return The world renderer.
    MCWorldRenderer & renderer() const;

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

    //! \return size of the current integration vector
    int objectCount() const;

    /*! \brief Set collision resolver loop count.
     *  Lower loop count results in faster collision calculations, but lower accuracy. */
    void setResolverLoopCount(MCUint resolverLoopCount = 5);

protected:

    //! Get registered objects
    ObjectVector objects() const;

private:

    DISABLE_COPY(MCWorld);
    DISABLE_ASSI(MCWorld);

    void integrate(int step);
    void processRemovedObjects();
    void processCollisions();
    void doRemoveObject(MCObject & object);
    void detectCollisions();
    void generateImpulses();
    void resolvePositions(MCFloat accuracy);
    MCContact * getDeepestInterpenetration(const std::vector<MCContact *> & contacts);

    static MCWorld      * m_instance;
    MCWorldRenderer     * m_renderer;
    MCForceRegistry     * m_forceRegistry;
    MCCollisionDetector * m_collisionDetector;
    MCImpulseGenerator  * m_impulseGenerator;
    MCObjectGrid        * m_objectGrid;
    static MCFloat        m_metersPerUnit;
    static MCFloat        m_metersPerUnitSquared;
    MCFloat               m_minX, m_maxX, m_minY, m_maxY, m_minZ, m_maxZ;
    MCWorld::ObjectVector m_objs;
    MCWorld::ObjectVector m_removeObjs;
    MCObject            * m_leftWallObject;
    MCObject            * m_rightWallObject;
    MCObject            * m_topWallObject;
    MCObject            * m_bottomWallObject;
    MCUint                m_numCollisions;
    MCUint                m_resolverLoopCount;
    MCFloat               m_resolverStep;
    MCVector3dF           m_gravity;
};

#endif // MCWORLD_HH
