// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCOBJECT_HH
#define MCOBJECT_HH

#include "mcbbox.hh"
#include "mccontact.hh"
#include "mcmacros.hh"
#include "mcobjectgrid.hh"
#include "mcshape.hh"
#include "mctyperegistry.hh"
#include "mctypes.hh"
#include "mcvector3d.hh"
#include "mcworld.hh"

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

class MCShapeView;
class MCSurface;
class MCEvent;
class MCCollisionEvent;
class MCOutOfBoundariesEvent;
class MCPhysicsComponent;
class MCTimerEvent;
class MCCamera;

typedef std::shared_ptr<MCObject> MCObjectPtr;

/*! \class MCObject.
 *  \brief MCObject is the base for all MiniCore objects.
 *
 *  MCObject encapsulates the physics and view properties of an object. */
class MCObject
{
public:

    //! Hash container for contacts. Prefer map here for iteration performance.
    typedef std::map<MCObject *, std::vector<MCContact *> > ContactHash;

    /*! Constructor.
     *  \param typeId Type ID string e.g. "MY_OBJECT_CLASS". */
    explicit MCObject(const std::string & typeId);

    /*! Constructor.
     *  Construct MCObject using the given shape.
     *  \param shape Pointer to the shape to be used.
     *  \param typeId Type ID string e.g. "MY_OBJECT_CLASS". */
    MCObject(MCShapePtr shape, const std::string & typeId);

    /*! Constructor.
     *  Construct MCObject implicitly using MCRectShape with
     *  MCSurfaceView for the given MCSurface.
     *  \param surface Pointer to the (shared) surface to be used.
     *  MCObject won't take the ownership, because the same surface
     *  can be used to draw multiple objects and is managed by MCSurfaceManager.
     *  \param typeId Type ID string e.g. "MY_OBJECT_CLASS".
     *  \param batchMode \see MCShapeView::setBatchMode(). */
    MCObject(MCSurface & surface, const std::string & typeId, bool batchMode = false);

    //! Return integer id corresponding to the given object name.
    static MCUint getTypeIDForName(const std::string & typeName);

    /*! Destructor. It's the callers responsibility to first remove
     *  the object from MCWorld before deleting the object. */
    virtual ~MCObject();

    /*! Return unique class-wide id that can be used to
     *  efficiently test the type of a derived classes
     *  against each others without need for dynamic_cast.
     *  The filtered collision detection system uses this
     *  to match given types of objects. */
    virtual MCUint typeID() const;

    /*! Return typeId for the given typeName string from
     *  MCObject's hash table. Each object registers
     *  its type in its constructor. This is automatic for
     *  derived classes, they only need to pass the desired
     *  typeId string to MCObject's constructor.
     *  Returns 0 if type is not registered. */
    static MCUint typeID(const std::string & typeName);

    /*! Send event to given object.
     *  \param object Destination object.
     *  \param event Event to be sent. */
    static void sendEvent(MCObject & object, MCEvent & event);

    //! Subscribe the given object to timer events.
    static void subscribeTimerEvent(MCObject & object);

    //! Subscribe the given object from timer events.
    static void unsubscribeTimerEvent(MCObject & object);

    /*! Send the given timer event to all objects that
     *  have subscribed to timer events. */
    static void sendTimerEvent(MCTimerEvent & event);

    /*! Render the object.
     *  \param p Camera window to be used. */
    virtual void render(MCCamera * p = nullptr);

    /*! Render the object shadow.
     *  \param p Camera window to be used. */
    virtual void renderShadow(MCCamera * p = nullptr);

    /*! \brief Sets whether the object should be rendered automatically.
     *  True is the default. */
    void setIsRenderable(bool flag);

    //! \brief Return whether the object should be automatically rendered.
    bool isRenderable() const;

    /*! \brief Add object to the World.
     *  Convenience method to add object to the MCWorld instance.
     *  Composite objects may override this and add all their sub-objects. */
    virtual void addToWorld();

    //! \brief Combined addToWorld() and translate.
    virtual void addToWorld(MCFloat x, MCFloat y, MCFloat z = 0);

    /*! \brief Remove object from the World.
     *  Convenience method to remove object from the MCWorld instance.
     *  Composite objects may re-implement this and remove all their sub-objects. */
    virtual void removeFromWorld();

    /*! \brief Remove object from the World immediately.
     *  Convenience method to remove object from the MCWorld instance.
     *  Composite objects may re-implement this and remove all their sub-objects. */
    virtual void removeFromWorldNow();

    /*! \brief Sets whether the physics of the object should be updated.
     *  True is the default. */
    void setIsPhysicsObject(bool flag);

    //! Return whether the object is a physics object.
    bool isPhysicsObject() const;

    /*! Override the default physics component.
     *  MCObject will take the ownership. */
    void setPhysicsComponent(MCPhysicsComponent & physicsComponent);

    /*! \brief The physics component of the object.
     *  All physics functionality is accessed via this component. */
    MCPhysicsComponent & physicsComponent();

    /*! \brief Sets whether the object behaves as a trigger object.
     *  Trigger object produces collision events even though it's not a
     *  physics object. False is the default. */
    void setIsTriggerObject(bool flag);

    //! Return whether the object is a trigger object.
    bool isTriggerObject() const;

    /*! \brief Sets whether the collision detection for this object is totally bypassed.
     *  False is the default. */
    void setBypassCollisions(bool flag);

    //! Return whether collisions are bypassed.
    bool bypassCollisions() const;

    //! Set the object as a particle.
    void setIsParticle(bool flag);

    //! Return true if the object is a particle.
    bool isParticle() const;

    /*! Set location
     *  \param newLocation The new location. */
    void translate(const MCVector3dF & newLocation);

    /*! Set location relative to parent.
     *  \param newLocation The new location. */
    void translateRelative(const MCVector3dF & newLocation);

    /*! Displace object
     *  \param displacement Translates object to location() + displacement. */
    void displace(const MCVector3dF & displacement);

    /*! Set offset for the fake shadow.
     *  \param p The new offset. Applies only if a shape has been set. */
    void setShadowOffset(const MCVector2dF & p);

    //! Return location
    const MCVector3dF & location() const;

    //! Return parent-relative location
    const MCVector3dF & relativeLocation() const;

    /*! Set rotation ("yaw") angle about Z-axis.
     *  \param newAngle The new angle in degrees [0..360]. */
    void rotate(MCFloat newAngle, bool updateChildTransforms = true);

    //! Set object's center in local coordinates i.e. (0, 0) is the default original center.
    void setCenter(MCVector2dF center);

    //! Get rotation angle.
    MCFloat angle() const;

    //! Get direction vector.
    MCVector2dF direction() const;

    //! Set shape.
    void setShape(MCShapePtr shape);

    //! Get shape.
    MCShapePtr shape() const;

    /*! \brief Step internal time.
     *  This is called AFTER every update step. Note that you might need to prevent
     *  the object from sleeping via MCPhysicsComponent if these events are required for
     *  an object that remains still.
     *  \param step Time step in msecs */
    virtual void onStepTime(int step);

    /*! Set collision layer. Only objects on the same layer can collide.
     * -1 will collide with all layers. The default is 0.
     * \param layer The new layer. */
    void setCollisionLayer(int layer);

    //! Return the collision layer.
    int collisionLayer() const;

    //! Add a collision contact.
    void addContact(MCContact & contact);

    //! Get collision contact hash.
    const ContactHash & contacts() const;

    //! Delete current contacts.
    void deleteContacts();

    //! Delete all contacts with the given object.
    void deleteContacts(MCObject & object);

    //! Return index in MCWorld's object vector. Returns -1 if not in the world.
    int index() const;

    //! Set initial location. This won't result in any translations.
    void setInitialLocation(const MCVector3dF & location);

    //! Get initial location.
    const MCVector3dF & initialLocation() const;

    //! Set initial angle. This won't result in any transformations.
    void setInitialAngle(int angle);

    //! Get initial angle.
    int initialAngle() const;

    //! Add child object. Used on composite objects.
    void addChildObject(
        MCObjectPtr object,
        const MCVector3dF & relativeLocation = MCVector3dF(),
        MCFloat relativeAngle = 0);

    //! Remove child object.
    void removeChildObject(MCObject & object);

    //! Remove child object.
    void removeChildObject(MCObjectPtr object);

    typedef std::vector<MCObjectPtr> Children;
    const Children & children() const;

    //! Get parent or self;
    MCObject & parent() const;

    /*! Set parent relative rotation ("yaw") angle about Z-axis.
     *  \param newAngle The new angle in degrees [0..360]. */
    void rotateRelative(MCFloat newAngle);

    void checkBoundaries();

protected:

    /*! Event handler.
     *  \param pEvent Event to be handled.
     *  \return true if event was handled. */
    virtual bool event(MCEvent & event);

    /*! Event handler for MCCollisionEvent. Override this to
     *  filter out collisions with certain objects.
     *  All collisions are accepted by default.
     *  \param event Event to be handled. */
    virtual void collisionEvent(MCCollisionEvent & event);

    /*! Event handler for MCOutOfBoundariesEvent.
     *  \param event Event to be handled. */
    virtual void outOfBoundariesEvent(MCOutOfBoundariesEvent & event);

private:

    MCFloat calculateLinearBalance(const MCVector3dF & force, const MCVector3dF & pos);

    /*! Cache range of objectGrid cells the object is touching.
     *  Used by MCObjectGrid. */
    void cacheIndexRange(MCUint i0, MCUint i1, MCUint j0, MCUint j1);

    void checkXBoundariesAndSendEvent(MCFloat minX, MCFloat maxX);

    void checkYBoundariesAndSendEvent(MCFloat minY, MCFloat maxY);

    void checkZBoundariesAndSendEvent();

    void doRotate(MCFloat newAngle);

    void rotateShape(MCFloat angle);

    /*! Return true, if object is to be removed.
     *  Used by MCWorld. */
    bool removing() const;

    /*! Get cached index range.
     *  Used by MCObjectGrid. */
    void restoreIndexRange(MCUint * i0, MCUint * i1, MCUint * j0, MCUint * j1);

    /*! Set index in worlds' object vector.
     *  Used by MCWorld. */
    void setIndex(int index);

    //! Set parent object. Used on composite objects.
    void setParent(MCObject & parent);

    /*! Set object to be removed. Objects cannot be removed immediately, because
     *  they might be involved in collision calculations of other objects yet to
     *  be completed. Used by MCWorld. */
    void setRemoving(bool flag);

    void updateChildTransforms();

    void updateCenter();

    void setStatus(int bit, bool flag);

    bool testStatus(int bit) const;

    static MCTypeRegistry        m_typeRegistry;
    MCUint                       m_typeID;
    MCFloat                      m_angle = 0; // Degrees
    MCFloat                      m_relativeAngle = 0; // Degrees
    int                          m_collisionLayer = 0;
    int                          m_index = -1;
    MCUint                       m_i0 = 0;
    MCUint                       m_i1 = 0;
    MCUint                       m_j0 = 0;
    MCUint                       m_j1 = 0;
    MCVector3dF                  m_initialLocation;
    int                          m_initialAngle = 0;
    MCVector3dF                  m_location;
    MCVector3dF                  m_relativeLocation;
    MCVector2dF                  m_initialCenter;
    MCVector2dF                  m_center;
    bool                         m_centerIsZero = false;
    MCShapePtr                   m_shape;
    typedef std::vector<MCObject * > TimerEventObjectsList;
    static TimerEventObjectsList m_timerEventObjects;
    MCObject::ContactHash        m_contacts;
    int                          m_timerEventObjectsIndex = -1;
    int                          m_status;
    Children                     m_children;
    MCObject *                   m_parent;

    MCPhysicsComponent *         m_physicsComponent;

    //! Disable copy constructor and assignment.
    DISABLE_COPY(MCObject);
    DISABLE_ASSI(MCObject);

    friend class MCObjectGrid;
    friend class MCObjectGridImpl;
    friend class MCWorld;
    friend class MCCollisionDetector;
};

#endif // MCOBJECT_HH
