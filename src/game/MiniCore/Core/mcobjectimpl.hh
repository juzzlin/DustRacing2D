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

#ifndef MCOBJECTIMPL_HH
#define MCOBJECTIMPL_HH

#include "mcvector3d.hh"
#include <unordered_map>
#include <string>

//! Implementation class for MCObject.
class MCObjectImpl
{
private:
    MCObjectImpl(MCObject * pPublic, const std::string & typeName);
    ~MCObjectImpl();
    static MCUint getTypeIDForName(const std::string & typeName);
    static MCUint registerType(const std::string & typeName);
    inline void integrate(MCFloat step);
    inline void doOutOfBoundariesEvent();
    void setFlag(MCUint flag, bool enable);
    MCObject * const pPublic;
    MCUint typeID;
    MCUint time;
    MCFloat invMass;
    MCFloat mass;
    MCFloat restitution;
    MCFloat xyFriction;
    MCUint angle;
    MCFloat maximumVelocity;
    MCUint layer;
    int index;
    MCUint flags;
    MCUint i0, i1, j0, j1;
    MCVector3d<MCFloat> acceleration;
    MCVector3d<MCFloat> velocity;
    MCVector3d<MCFloat> initialLocation;
    MCVector3d<MCFloat> location;
    MCVector3d<MCFloat> forces;
    MCShape * pShape;
    typedef std::unordered_map<std::string, MCUint> TypeHash;
    static TypeHash typeHash;
    static MCUint typeIDCount;
    MCObject::ContactHash contacts;
    friend class MCObject;
};

#endif // MCOBJECTIMPL_HH
