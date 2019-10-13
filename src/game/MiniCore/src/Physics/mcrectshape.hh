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

#ifndef MCRECTSHAPE_HH
#define MCRECTSHAPE_HH

#include "mcedge.hh"
#include "mcobbox.hh"
#include "mcsegment.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"

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
     * \param view View for the shape. May be nullptr. */
    MCRectShape(MCShapeViewPtr view, float width, float height);

    //! Destructor
    virtual ~MCRectShape() override;

    //! \reimp
    virtual void translate(const MCVector3dF & p) override;

    //! \reimp
    virtual void rotate(float a) override;

    //! \reimp
    virtual MCBBoxF bbox() const override;

    //! \reimp
    virtual bool contains(const MCVector2dF & p) const override;

    //! \reimp
    virtual float interpenetrationDepth(const MCSegmentF & p, MCVector2dF & contactNormal) const override;

    //! \reimp
    virtual MCVector2dF contactNormal(const MCSegmentF & p) const override;

    //! \brief Resize
    void resize(float width, float height);

    //! Return the oriented bbox to access vertices etc.
    const MCOBBoxF & obbox() const;

    //! Get crossing edge for the given segment.
    MCEdgeF edgeForSegment(const MCSegmentF & p) const;

    //! \reimp
    virtual MCShape::Type type() const override;

    //! \reimp
    virtual void render(MCCamera * p) override;

    //! Return width.
    float width() const;

    //! Return height.
    float height() const;

private:
    DISABLE_COPY(MCRectShape);
    DISABLE_ASSI(MCRectShape);

    inline MCVector2dF contactNormal(const MCSegmentF & p, const MCEdgeF & e) const;

    MCOBBox<float> m_obbox;

    float m_width;

    float m_height;
};

#endif // MCRECTSHAPE_HH
