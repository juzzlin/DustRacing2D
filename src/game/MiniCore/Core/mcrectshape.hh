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

#ifndef MCRECTSHAPE_HH
#define MCRECTSHAPE_HH

#include "mcshape.hh"
#include "mcobbox.hh"
#include "mcedge.hh"
#include "mcsegment.hh"

class MCShapeView;
class MCObject;

/*! \class MCRectShape
 *  \brief Rectangular shape model
 *
 * Rectangular (oriented bbox) shape model for an MCObject.
 */
class MCRectShape : public MCShape
{
public:

    /*! Constructor
     * \param pView View for the shape. May be nullptr.
     */
    MCRectShape(MCShapeView * pView, MCFloat width, MCFloat height);

    //! Destructor
    virtual ~MCRectShape();

    //! \reimp
    virtual void translate(const MCVector3dF & p);

    //! \reimp
    virtual void rotate(MCFloat a);

    //! \reimp
    virtual MCBBoxF bbox() const;

    //! \reimp
    virtual bool contains(const MCVector2dF & p) const;

    //! \reimp
    virtual int interpenetrationDepth(const MCSegmentF & p, MCVector2dF & contactNormal) const;

    //! \reimp
    virtual MCVector2dF contactNormal(const MCSegmentF & p) const;

    //! \brief Resize
    void resize(MCFloat width, MCFloat height);

    //! Return the oriented bbox to access vertices etc.
    const MCOBBoxF & obbox() const;

    //! Get crossing edge for the given segment.
    MCEdgeF edgeForSegment(const MCSegmentF & p) const;

    //! Return the typeID
    static MCUint typeID();

    //! \reimp
    virtual MCUint instanceTypeID() const;

    //! \reimp
    virtual void render(MCCamera * p);

    //! \reimp
    virtual void renderScaled(MCFloat wr, MCFloat hr, MCCamera * p);

    //! Return approximated radius.
    virtual MCFloat radius() const;

    //! Return width.
    MCFloat width() const;

    //! Return height.
    MCFloat height() const;

private:

    DISABLE_COPY(MCRectShape);
    DISABLE_ASSI(MCRectShape);

    static MCUint m_typeID;
    inline MCVector2dF contactNormal(const MCSegmentF & p, const MCEdgeF & e) const;
    MCOBBox<MCFloat> m_obbox;
    MCFloat m_momentOfInertiaFactor;
    MCFloat m_radius;
    MCFloat m_width;
    MCFloat m_height;
};

#endif // MCRECTSHAPE_HH
