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

#include "mcsurfaceobjectdata.hh"

//! Private class of MCSurfaceObjectData.
class MCSurfaceObjectDataImpl
{
private:
    MCSurfaceObjectDataImpl();
    std::string surfaceId;
    bool defaultCircleShape;
    friend class MCSurfaceObjectData;
};

MCSurfaceObjectDataImpl::MCSurfaceObjectDataImpl()
: surfaceId("")
, defaultCircleShape(false)
{
}

MCSurfaceObjectData::MCSurfaceObjectData(const std::string & typeId)
: MCObjectData(typeId)
, m_pImpl(new MCSurfaceObjectDataImpl)
{
}

void MCSurfaceObjectData::setSurfaceId(const std::string & id)
{
    m_pImpl->surfaceId = id;
}

const std::string & MCSurfaceObjectData::surfaceId() const
{
    return m_pImpl->surfaceId;
}

void MCSurfaceObjectData::setDefaultCircleShape(bool state)
{
    m_pImpl->defaultCircleShape = state;
}

bool MCSurfaceObjectData::defaultCirleShape() const
{
    return m_pImpl->defaultCircleShape;
}

MCSurfaceObjectData::~MCSurfaceObjectData()
{
    delete m_pImpl;
}
