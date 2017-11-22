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

#ifndef MCCIRCLESHAPE_HH
#define MCCIRCLESHAPE_HH

#include "mcshape.hh"
#include "mcshapeview.hh"

//! Circular shape model
class MCCircleShape : public MCShape
{
public:

    /*! Constructor
     * \param radius Radius of the shape
     * \param view View for the shape. May be nullptr. */
    MCCircleShape(MCShapeViewPtr view, float radius);

    //! Destructor
    virtual ~MCCircleShape();

    //! \reimp
    bool contains(const MCVector2d<float> & p) const;

    //! \reimp
    float interpenetrationDepth(const MCSegment<float> & p, MCVector2dF & contactNormal) const;

    //! Specialized version to get interpenetration depth and contact normal.
    float interpenetrationDepth(const MCCircleShape & p, MCVector2dF & contactNormal) const;

    //! \reimp
    MCVector2d<float> contactNormal(const MCSegment<float> & p) const;

    //! Return type id
    static unsigned int typeId();

    //! \reimp
    virtual unsigned int instanceTypeId() const;

    //! \reimp
    virtual MCBBox<float> bbox() const;

private:

    DISABLE_COPY(MCCircleShape);
    DISABLE_ASSI(MCCircleShape);

    static unsigned int m_typeId;
    float m_radius;
};

#endif // MCCIRCLESHAPE_HH
