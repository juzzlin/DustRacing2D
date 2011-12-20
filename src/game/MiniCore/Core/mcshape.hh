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
#include "mcmacros.hh"
#include "mctypes.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <memory>

class MCShapeImpl;
class MCShapeView;
class MCObject;
class MCCamera;

/*! \class MCShape.
 *  \brief MCShape is abstract base class for different shape models used by MCObject.
 */
class MCShape
{
public:

    /*! Constructor.
     * \param pParent Parent / owner object of the shape.
     * \param pView View for the shape. MCShape takes the ownership.
     */
    explicit MCShape(MCObject * pParent, MCShapeView * pView = nullptr);

    //! Destructor.
    virtual ~MCShape();

    //! Get the parent object.
    MCObject * parent() const;

    //! Set view object. MCShape takes the ownership.
    void setView(MCShapeView * p);

    /*! Set view object. Use this if you want to share the same view
     *  between multiple objects;
     */
    void setView(std::shared_ptr<MCShapeView> p);

    //! Get view class.
    MCShapeView * view() const;

    /*! Render.
     * \param p Camera window to be used
     */
    virtual void render(MCCamera * p = nullptr);

    /*! Render shadow.
     * \param p Camera window to be used
     */
    virtual void renderShadow(MCCamera * p = nullptr);

    /*! Render scaled.
     * \param p Camera window to be used
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     */
    virtual void renderScaled(MCFloat wr, MCFloat hr, MCCamera * p = nullptr);

    /*! Render shadow scaled.
     * \param p Camera window to be used
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     */
    virtual void renderShadowScaled(MCFloat wr, MCFloat hr, MCCamera * p = nullptr);

    /*! Translate.
     * \param p The new location
     */
    virtual void translate(const MCVector3d<MCFloat> & p);

    //! Get the current location.
    const MCVector3d<MCFloat> & location() const;

    /*! Set offset for the fake shadow.
     * \param p The new offset.
     */
    void setShadowOffset(const MCVector2d<MCFloat> & p);

    /*! Rotate.
     * \param a The new rotation angle in degrees
     */
    virtual void rotate(MCUint a);

    //! Return the current angle.
    MCUint angle() const;

    //! Return non-rotated bounding box of the shape.
    virtual MCBBox<MCFloat> bbox() const = 0;

    /*! Tests if shape contains the given point.
     * \param p The point to be tested
     * \return True if point contained
     */
    virtual bool contains(const MCVector2d<MCFloat> & p) const = 0;

    /*! \brief Returns the interpenetration depth for the given point.
     * Note!!: It must be first tested, that p1 is inside the shape.
     * \param p1 The interpenetrated point to be tested
     * \param p2 Control point e.g. the center of the colliding object
     * \return Interpenetration depth.
     */
    virtual int interpenetrationDepth(const MCVector2d<MCFloat> & p1, const MCVector2d<MCFloat> & p2) const = 0;

    /*! Returns contact normal for the given point
     * \param p The point to be tested. It can be outside or inside the shape.
     * \return Contact normal pointing outwards the shape
     */
    virtual MCVector2d<MCFloat> contactNormal(const MCVector2d<MCFloat> & p) const = 0;

    /*! Register a new event type
     * \return The new unique type ID
     */
    static MCUint registerType();

    //! Return class-wide type id
    virtual MCUint instanceTypeID() const = 0;

private:

    //! Disable copy constructor and assignment
    DISABLE_COPY(MCShape);
    DISABLE_ASSI(MCShape);
    MCShapeImpl * const m_pImpl;
};

#endif // MCSHAPE_HH
