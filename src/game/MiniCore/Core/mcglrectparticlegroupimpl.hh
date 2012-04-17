// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCGLRECTPARTICLEGROUPIMPL_HH
#define MCGLRECTPARTICLEGROUPIMPL_HH

#include <vector>
#include <unordered_set>

//! Implementation class for MCGLRectParticleGroup
class MCGLRectParticleGroupImpl
{
public:
    MCGLRectParticleGroupImpl();
    ~MCGLRectParticleGroupImpl();

private:
    friend class MCGLRectParticleGroup;
    std::vector<MCGLRectParticle *> particles;
    std::unordered_set<MCGLRectParticle *> addedParticles;
};

#endif // MCGLRECTPARTICLEGROUPIMPL_HH
