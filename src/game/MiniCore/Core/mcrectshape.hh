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

class MCRectShapeImpl;
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
     * \param parent Parent object.
     * \param pView View for the shape. May be nullptr.
     */
    MCRectShape(MCObject & parent, MCShapeView * pView,
        MCFloat width, MCFloat height);

    //! Destructor
    virtual ~MCRectShape();

    //! \reimp
    virtual void translate(const MCVector3d<MCFloat> & p);

    //! \reimp
    virtual void rotate(MCFloat a);

    //! \reimp
    virtual MCBBox<MCFloat> bbox() const;

    //! \reimp
    virtual bool contains(const MCVector2d<MCFloat> & p) const;

    //! \reimp
    virtual int interpenetrationDepth(const MCSegment<MCFloat> & p) const;

    //! \reimp
    virtual MCVector2d<MCFloat> contactNormal(const MCSegment<MCFloat> & p) const;

    //! \brief Resize
    void resize(MCFloat width, MCFloat height);

    //! Return the oriented bbox to access vertices etc.
    const MCOBBox<MCFloat> & obbox() const;

    //! Get crossing edge for the given segment.
    MCEdge<MCFloat> edgeForSegment(const MCSegment<MCFloat> & p) const;

    //! Return the typeID
    static MCUint typeID();

    //! \reimp
    virtual MCUint instanceTypeID() const;

    //! \reimp
    virtual void render(MCCamera * p);

    //! \reimp
    virtual void renderScaled(MCFloat wr, MCFloat hr, MCCamera * p);

    //! \reimp
    virtual MCFloat momentOfInertia() const;

    //! Return approximated radius.
    virtual MCFloat radius() const;

    //! Return width.
    MCFloat width() const;

    //! Return height.
    MCFloat height() const;

private:

    DISABLE_COPY(MCRectShape);
    DISABLE_ASSI(MCRectShape);
    MCRectShapeImpl * const m_pImpl;
};

#endif // MCRECTSHAPE_HH
