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

#ifndef MCSHAPE_HH
#define MCSHAPE_HH

#include "mcbbox.hh"
#include "mcbbox3d.hh"
#include "mcmacros.hh"
#include "mctypes.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"
#include "mcsegment.hh"
#include "mcshapeview.hh"

#include <memory>

class MCObject;
class MCCamera;

/*! \class MCShape.
 *  \brief MCShape is abstract base class for different shape models used by MCObject.
 */
class MCShape
{
public:

    /*! Constructor.
     * \param pView View for the shape. */
    explicit MCShape(MCShapeViewPtr view = nullptr);

    //! Destructor.
    virtual ~MCShape();

    //! Set the parent object. MCObject will call this automatically when a shape is set.
    void setParent(MCObject & parent);

    //! Get the parent object.
    MCObject & parent() const;

    /*! Set view object. The same view can be shared between multiple objects; */
    void setView(MCShapeViewPtr view);

    //! Get view object.
    MCShapeViewPtr view() const;

    /*! Render.
     * \param p Camera window to be used */
    virtual void render(MCCamera * p = nullptr);

    /*! Render shadow.
     * \param p Camera window to be used */
    virtual void renderShadow(MCCamera * p = nullptr);

    /*! Translate.
     * \param p The new location */
    virtual void translate(const MCVector3dF & p);

    //! Get the current location.
    const MCVector3dF & location() const;

    /*! Set offset for the fake shadow.
     * \param p The new offset. */
    void setShadowOffset(const MCVector3dF & p);

    /*! \return Set offset for the fake shadow. */
    const MCVector3dF & shadowOffset() const;

    /*! Set global default shadow offset
      * \param p The new offset. */
    static void setDefaultShadowOffset(const MCVector3dF & p);

    /*! Rotate.
     * \param a The new rotation angle in degrees */
    virtual void rotate(MCFloat a);

    //! Return the current angle.
    MCFloat angle() const;

    //! Return non-rotated, translated bounding box of the shape in 2d.
    virtual MCBBoxF bbox() const = 0;

    /*! Tests if shape contains the given point.
     * \param p The point to be tested
     * \return True if point contained */
    virtual bool contains(const MCVector2dF & p) const = 0;

    /*! \brief Returns the interpenetration depth for the given point.
     * Note!!: It must be first tested, that p.vertex0 is inside the shape.
     * \param p The crossing segment to be tested.
     * \return Interpenetration depth. Set contactNormal to the contactNormal. */
    virtual MCFloat interpenetrationDepth(const MCSegmentF & p, MCVector2dF & contactNormal) const = 0;

    /*! \brief Returns contact normal for the given point
     * Note!!: It must be first tested, that p.vertex0 is inside the shape.
     * \param p The crossing segment to be tested.
     * \return Contact normal pointing outwards the shape. */
    virtual MCVector2dF contactNormal(const MCSegmentF & p) const = 0;

    /*! Register a new shape type.
     * \return The new unique type ID. */
    static MCUint registerType();

    /*! Return class-wide static type id inited by calling
     *  MCShape::registerType(). This is used to optimize collision detection
     *  and to avoid dynamic_cast. */
    virtual MCUint instanceTypeID() const = 0;

    //! Return approximated radius.
    MCFloat radius() const;

    //! Fast intersection test
    bool mayIntersect(MCShape & other);

protected:

    void setRadius(MCFloat radius);

private:

    //! Disable copy constructor and assignment
    DISABLE_COPY(MCShape);
    DISABLE_ASSI(MCShape);

    static MCUint m_typeCount;

    MCObject * m_parent;

    MCVector3dF m_location;

    MCVector3dF m_shadowOffset;

    static MCVector3dF m_defaultShadowOffset;

    MCFloat m_angle;

    MCFloat m_radius;

    MCShapeViewPtr m_view;
};

typedef std::shared_ptr<MCShape> MCShapePtr;

#endif // MCSHAPE_HH
