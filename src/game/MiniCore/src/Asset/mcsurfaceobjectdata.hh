// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCSURFACEOBJECTDATA_HH
#define MCSURFACEOBJECTDATA_HH

#include "mcobjectdata.hh"
#include <MCVector3d>

class MCSurfaceObjectDataImpl;

//! Surface object description that can be given to MCObjectFactory.
class MCSurfaceObjectData : public MCObjectData
{
public:

    //! Constructor.
    explicit MCSurfaceObjectData(const std::string & typeId);

    //! Destructor.
    virtual ~MCSurfaceObjectData() {};

    //! Set the surface id.
    void setSurfaceId(const std::string & id);

    //! Return the surface id.
    const std::string & surfaceId() const;

    //! Sets the shape to be a circle according to the surface dimensions.
    void setDefaultCircleShape(bool state);

    //! Returns true if default circle shape is wanted.
    bool defaultCirleShape() const;

    //! Sets whether the surface is rendered in batch mode.
    void setBatchMode(bool state);

    //! Returns whether the surface is rendered in batch mode.
    bool batchMode() const;

private:

    DISABLE_COPY(MCSurfaceObjectData);
    DISABLE_ASSI(MCSurfaceObjectData);

    std::string m_surfaceId;
    bool        m_defaultCircleShape;
    bool        m_batchMode;
};

#endif // MCSURFACEOBJECTDATA_HH
