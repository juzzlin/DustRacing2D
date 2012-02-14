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

#ifndef MCSHAPEIMPL_HH
#define MCSHAPEIMPL_HH

#include <memory>

class MCShapeView;
class MCObject;

//! Implementation class for MCShape
class MCShapeImpl
{
protected:
    explicit MCShapeImpl(MCObject & parent);
    virtual ~MCShapeImpl();
private:
    static MCUint m_typeCount;
    MCObject & m_parent;
    MCVector3d<MCFloat> m_location;
    MCVector2d<MCFloat> m_shadowOffset;
    MCFloat m_angle;
    std::shared_ptr<MCShapeView> m_pView;
    friend class MCShape;
};

#endif // MCSHAPEIMPL_HH
