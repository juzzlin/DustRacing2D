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

#ifndef MCOBJECT_HH
#define MCOBJECT_HH

#include "mcbbox.hh"
#include "mccontact.hh"
#include "mcmacros.hh"
#include "mcobjecttree.hh"
#include "mctypes.hh"
#include "mcvector3d.hh"
#include "mcworld.hh"

#include <map>
#include <unordered_map>
#include <vector>
#include <string>

class MCShape;
class MCShapeView;
class MCSurface;
class MCEvent;
class MCCollisionEvent;
class MCOutOfBoundariesEvent;
class MCTimerEvent;
class MCCamera;

//! \class MCObject.
//! \brief MCObject is the base for all MiniCore objects.
//!
//! MCObject encapsulates the physics and view properties of an object.
class MCObject
{
public:

    //! Hash container for contacts. Prefer map here for iteration performance.
    typedef std::map<MCObject *, std::vector<MCContact *> > ContactHash;

    //! Constructor.
    //! \param typeId Type ID string e.g. "MY_OBJECT_CLASS".
    explicit MCObject(const std::string & typeId);

    //! Constructor.
    //! Construct MCObject using the given shape.
    //! \param pShape Pointer to the surface to be used.
    //! MCObject will take the ownership.
    //! \param typeId Type ID string e.g. "MY_OBJECT_CLASS".
    MCObject(MCShape * pShape, const std::string & typeId);

    //! Constructor.
    //! Construct MCObject implicitly using MCRectShape with
    //! MCSurfaceView for the given MCSurface.
    //! \param pSurface Pointer to the (shared) surface to be used.
    //! MCObject won't take the ownership, because the same surface
    //! can be used to draw multiple objects.
    //! \param typeId Type ID string e.g. "MY_OBJECT_CLASS".
    MCObject(MCSurface * pSurface, const std::string & typeId);

    //! Destructor. It's the callers responsibility to first remove
    //! the object from MCWorld before deleting the object.
    virtual ~MCObject();

    //! Return unique class-wide id that can be used to
    //! efficiently test the type of a derived classes
    //! against each others without need for dynamic_cast.
    //! The filtered collision detection system uses this
    //! to match given types of objects.
    virtual MCUint typeID() const;

    //! Return typeId for the given typeName string from
    //! MCObject's hash table. Each object registers
    //! its type in its constructor. This is automatic for
    //! derived classes, they only need to pass the desired
    //! typeId string to MCObject's constructor.
    //! Returns 0 if type is not registered.
    static MCUint typeID(const std::string & typeName);

    //! Send event to given object.
    //! \param object Destination object.
    //! \param event Event to be sent.
    static void sendEvent(MCObject & object, MCEvent & event);

    //! Subscribe the given object to timer events.
    static void subscribeTimerEvent(MCObject & object);

    //! Subscribe the given object from timer events.
    static void unsubscribeTimerEvent(MCObject & object);

    //! Send the given timer event to all objects that
    //! have subscribed to timer events.
    static void sendTimerEvent(MCTimerEvent & event);

    //! Render the object.
    //! \param p Camera window to be used.
    virtual void render(MCCamera * p = nullptr);

    //! Render the object shadow.
    //! \param p Camera window to be used.
    virtual void renderShadow(MCCamera * p = nullptr);

    //! \brief Add object to the World.
    //! Convenience method to add object to the MCWorld instance.
    //! Composite objects may override this and add all their sub-objects.
    virtual void addToWorld();

    //! \brief Combined addToWorld() and translate.
    virtual void addToWorld(MCFloat x, MCFloat y, MCFloat z = 0);

    //! \brief Remove object from the World.
    //! Convenience method to remove object from the MCWorld instance.
    //! Composite objects may re-implement this and remove all their sub-objects.
    virtual void removeFromWorld();

    //! \brief Remove object from the World immediately.
    //! Convenience method to remove object from the MCWorld instance.
    //! Composite objects may re-implement this and remove all their sub-objects.
    virtual void removeFromWorldNow();

    //! Set mass.
    //! \param newMass The new mass.
    //! \param stationary Sets inverse mass to zero if true.
    //!        This results in "infinite" mass for stationary objects.
    void setMass(MCFloat newMass, bool stationary = false);

    //! Get inverse mass.
    MCFloat invMass() const;

    //! Get mass.
    MCFloat mass() const;

    //! Return true if object is stationary.
    bool stationary() const;

    //! \brief Set maximum velocity.
    //! \param maxVelocity The new max velocity in length units / sec.
    //!                    Usually pixels / sec.
    void setMaximumVelocity(MCFloat maxVelocity);

    //! \brief Add velocity component of an impulse vector.
    //! \param impulse The velocity component.
    void addLinearImpulse(const MCVector3dF & impulse);

    //! Add rotational impulse in rad/s.
    void addAngularImpulse(MCFloat impulse);

    //! Set current velocity to the given value.
    //! \param newVelocity The new velocity.
    void setVelocity(const MCVector3dF & newVelocity);

    //! Return current velocity.
    const MCVector3dF & velocity() const;

    //! Set current angular velocity to the given value.
    //! Unit: rad/s.
    //! \param newVelocity The new velocity.
    void setAngularVelocity(MCFloat newVelocity);

    //! Return current angular velocity.
    //! Unit: rad/s.
    MCFloat angularVelocity() const;

    //! \brief Set maximum angular velocity.
    //! \param maxVelocity The new angular max velocity in rad/s.
    void setMaximumAngularVelocity(MCFloat maxVelocity);

    //! Set constant acceleration (e.g. gravity). Use addForce()
    //! to cause acceleration that varies from frame to frame.
    //! \param newAcceleration The new acceleration.
    void setAcceleration(const MCVector3dF & newAcceleration);

    //! Return constant acceleration.
    const MCVector3dF & acceleration() const;

    //! Add a force (N) vector to the object for a single frame.
    //! \param force Force vector to be added.
    void addForce(const MCVector3dF & force);

    //! Add a moment (Nm) to the object for a single frame.
    //! \param moment Value to be added.
    void addTorque(MCFloat moment);

    //! Set moment of inertia.
    void setMomentOfInertia(MCFloat momentOfInertia);

    //! Get moment of inertia.
    MCFloat momentOfInertia() const;

    //! Get inverse moment of inertia.
    MCFloat invMomentOfInertia() const;

    //! Clear accumulated forces.
    void clearForces();

    //! Resets all forces + velocities.
    void resetMotion();

    //! \brief Sets whether the physics of the object should be updated.
    //! True is the default.
    void setPhysicsObject(bool flag);

    //! \brief Return whether the object is a physics object.
    bool physicsObject() const;

    //! \brief Sets whether the collision detection for this object is totally bypassed.
    //! False is the default.
    void setBypassCollisions(bool flag);

    //! \brief Return whether collisions are bypassed.
    bool bypassCollisions() const;

    //! \brief Sets whether the object should be rendered automatically.
    //! True is the default.
    void setRenderable(bool flag);

    //! \brief Return whether the object should be automatically rendered.
    bool renderable() const;

    //! Return true if the object is a particle.
    bool isParticle() const;

    //! Set the object as a particle.
    void setIsParticle(bool flag);

    //! Set location
    //! \param newLocation The new location
    void translate(const MCVector3dF & newLocation);

    //! Displace object
    //! \param displacement Translates object to location() + displacement.
    void displace(const MCVector3dF & displacement);

    //! Set offset for the fake shadow.
    //! \param p The new offset. Applies only if a shape has been set.
    void setShadowOffset(const MCVector2dF & p);

    //! Return location
    const MCVector3dF & location() const;

    //! Convenience getter for location.
    MCFloat getX() const;

    //! Convenience getter for location.
    MCFloat getY() const;

    //! Convenience getter for location.
    MCFloat getZ() const;

    //! Set rotation ("yaw") angle about Z-axis.
    //! \param newAngle The new angle in degrees [0..360].
    void rotate(MCFloat newAngle);

    //! Set center of rotation in world coordinates.
    void setCenterOfRotation(MCVector2dF center);

    //! Get rotation angle.
    MCFloat angle() const;

    //! Get direction vector.
    MCVector2dF direction() const;

    //! \brief Set restitution.
    //! \param newRestitution The new restitution [0.0..1.0]
    //! (0.0 means a totally "soft" object, 1.0 means hard).
    //! Default is 0.5.
    void setRestitution(MCFloat newRestitution);

    //! Get restitution.
    MCFloat restitution() const;

    //! Set global friction. A friction coeff > 0 results in a
    //! implicit creation of a MCFrictionGenarator when the
    //! object is being added to the world.
    //! Use a custom friction generator if a more controlled
    //! friction is wanted.
    void setXYFriction(MCFloat friction);

    //! Get global friction.
    MCFloat xyFriction() const;

    //! Set shape. MCObject will take the ownership.
    void setShape(MCShape * newShape);

    //! Get shape.
    MCShape * shape() const;

    //! Set view. MCObject will take the ownership.
    void setView(MCShapeView * newView);

    //! \brief Step internal time.
    //! This is called AFTER every integration step. Must call base-class'
    //! implementation in the overidden version.
    virtual void stepTime();

    //! Return internal time.
    MCUint time() const;

    //! Reset internal time.
    void resetTime();

    //! Get view
    MCShapeView * view() const;

    //! Integrate forces. Usually called by MCWorld.
    //! \param step Time step to be integrated.
    void integrate(MCFloat step);

    //! Return current bounding box. Default implementation returns
    //! the bbox of the shape if set, and (0, 0, 1, 1) otherwise.
    MCBBox<MCFloat> bbox() const;

    //! Set rendering layer (0 is considered the lowest and is rendered
    //! first. This can be used when creating complex objects from
    //! 2d-surfaces only.
    //! \param layer The new layer.
    //! \param updateWorld Updates MCWorld's layer
    //! structure if true. Set to false if setting only the initial
    //! value and MCWorld instance is not created yet.
    void setLayer(MCUint layer, bool updateWorldLayers = true);

    //! Return the render layer.
    MCUint layer() const;

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

    //! Set the damping factor used in physics calculations. The default is 0.999.
    //! Use smaller value if the object is too unstable.
    void setDamping(MCFloat value);

    //! Return true, if the obejct is sleeping.
    bool sleeping() const;

    //! Object goes to sleep if linear and angular velocities drops below
    //! these values. The defaults are 0.01.
    void setSleepLimits(MCFloat linearSleepLimit, MCFloat angularSleepLimit);

    //! Return integer id corresponding to the given object name.
    static MCUint getTypeIDForName(const std::string & typeName);

protected:

    //! Event handler.
    //! \param pEvent Event to be handled.
    //! \return true if event was handled.
    virtual bool event(MCEvent & event);

    //! Event handler for MCCollisionEvent. Override this to
    //! filter out collisions with certain objects.
    //! All collisions are accepted by default.
    //! \param event Event to be handled.
    virtual void collisionEvent(MCCollisionEvent & event);

    //! Event handler for MCOutOfBoundariesEvent.
    //! \param event Event to be handled.
    virtual void outOfBoundariesEvent(MCOutOfBoundariesEvent & event);

    //! \brief Register a new type and get a unique type id.
    static MCUint registerType(const std::string & typeName);

private:

    //! TODO: Replace this with constructor chaining when GCC supports.
    void init(const std::string & typeId);

    //! Set index in worlds' object vector.
    //! Used by MCWorld.
    void setIndex(int index);

    //! Cache range of objectTree cells the object is touching.
    //! Used by MCObjectTree.
    void cacheIndexRange(MCUint i0, MCUint i1, MCUint j0, MCUint j1);

    //! Get cached index range.
    //! Used by MCObjectTree.
    void restoreIndexRange(MCUint * i0, MCUint * i1, MCUint * j0, MCUint * j1);

    //! Set object to be removed. Objects cannot be removed immediately, because
    //! they might be involved in collision calculations of other objects yet to
    //! be completed. Used by MCWorld.
    void setRemoving(bool flag);

    //! Return true, if object is to be removed.
    //! Used by MCWorld.
    bool removing() const;

    void toggleSleep(bool state);
    void integrateLinear(MCFloat step);
    void integrateAngular(MCFloat step);
    void doOutOfBoundariesEvent();
    void checkXBoundariesAndSendEvent(MCFloat minX, MCFloat maxX);
    void checkYBoundariesAndSendEvent(MCFloat minY, MCFloat maxY);
    void checkZBoundariesAndSendEvent();
    void setFlag(MCUint flag, bool enable);
    void doRotate(MCFloat newAngle);

    MCUint m_typeID;
    MCUint m_time;
    MCFloat m_invMass;
    MCFloat m_mass;
    MCFloat m_restitution;
    MCFloat m_xyFriction;
    MCFloat m_angle; // Degrees
    MCFloat m_angularAcceleration; // Radians / s^2
    MCFloat m_angularVelocity; // Radians / s
    MCFloat m_angularImpulse;
    MCFloat m_maximumAngularVelocity; // Radians / s
    MCFloat m_maximumVelocity;
    MCFloat m_torque;
    MCFloat m_invMomentOfInertia;
    MCFloat m_momentOfInertia;
    MCUint m_layer;
    int m_index;
    MCUint flags;
    MCUint m_i0, m_i1, m_j0, m_j1;
    MCVector3dF m_acceleration;
    MCVector3dF m_velocity;
    MCVector3dF m_linearImpulse;
    MCVector3dF m_initialLocation;
    int m_initialAngle;
    MCVector3dF m_location;
    MCVector3dF m_forces;
    MCVector2dF centerOfRotation;
    MCShape * pShape;
    typedef std::unordered_map<std::string, MCUint> TypeHash;
    static TypeHash typeHash;
    typedef std::vector<MCObject * > TimerEventObjectsList;
    static TimerEventObjectsList timerEventObjects;
    static MCUint typeIDCount;
    MCObject::ContactHash m_contacts;
    MCFloat damping;
    int timerEventObjectsIndex;
    bool m_sleeping;
    MCFloat m_linearSleepLimit;
    MCFloat m_angularSleepLimit;
    bool m_physicsObject;
    bool m_stationary;
    bool m_renderable;
    bool m_bypassCollisions;
    bool m_hasShadow;
    bool m_removing;
    bool m_renderOutline;
    bool m_isParticle;

    //! Disable copy constructor and assignment.
    DISABLE_COPY(MCObject);
    DISABLE_ASSI(MCObject);

    friend class MCObjectTree;
    friend class MCObjectTreeImpl;
    friend class MCWorld;
    friend class MCCollisionDetector;
};

#endif // MCOBJECT_HH
