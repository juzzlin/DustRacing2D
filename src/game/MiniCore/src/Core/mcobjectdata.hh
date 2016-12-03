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

#ifndef MCOBJECTDATA_HH
#define MCOBJECTDATA_HH

#include "mcmacros.hh"
#include "mctypes.hh"
#include <string>

#include <MCVector3d>

class MCObjectDataImpl;

//! Base class for object data classes. MCObjectData is used by the
//! MCObjectFactory in order to create objects using desired
//! configurations.
class MCObjectData
{
public:

    /*! Possible shape types:
     *  Rect   = MCRectShape.
     *  Circle = MCCircleShape. */
    enum Shape {Default, Rect, Circle};

    //! Constructor.
    explicit MCObjectData(const std::string & typeId);

    //! Destructor.
    virtual ~MCObjectData() {};

    //! Get type id.
    std::string typeId() const;

    //! Set mass.
    void setMass(MCFloat mass);

    //! Get mass.
    MCFloat mass() const;

    //! Set shape radius (implicitly selects circle shape).
    void setShapeRadius(MCFloat radius);

    //! Get shape radius.
    MCFloat shapeRadius() const;

    //! Set shape width (implicitly selects rect shape).
    void setShapeWidth(MCFloat width);

    //! Return shape width.
    MCFloat shapeWidth() const;

    //! Set shape height (implicitly selects rect shape).
    void setShapeHeight(MCFloat height);

    //! Return shape height.
    MCFloat shapeHeight() const;

    //! Return shape type.
    Shape shape() const;

    //! Set restitution.
    void setRestitution(MCFloat restitution);

    //! Get restitution.
    MCFloat restitution() const;

    //! Set global friction.
    void setXYFriction(MCFloat friction);

    //! Get global friction.
    MCFloat xyFriction() const;

    //! Set stationary.
    void setIsStationary(bool stationary);

    //! Return true if stationary.
    bool stationary() const;

    //! Set the initial location.
    void setInitialLocation(MCVector3dF location);

    //! Get the initial location.
    MCVector3dF initialLocation() const;

    //! Set the initial angle.
    void setInitialAngle(int angle);

    //! Get the initial angle.
    int initialAngle() const;

private:

    DISABLE_COPY(MCObjectData);
    DISABLE_ASSI(MCObjectData);

    std::string         m_typeId;
    MCFloat             m_mass;
    MCObjectData::Shape m_shape;
    MCFloat             m_shapeRadius;
    MCFloat             m_shapeWidth;
    MCFloat             m_shapeHeight;
    MCFloat             m_restitution;
    MCFloat             m_xyFriction;
    bool                m_stationary;
    MCVector3dF         m_location;
    int                 m_angle;
};

#endif // MCOBJECTDATA_HH
