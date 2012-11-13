// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef PARTICLEMANAGER_HPP
#define PARTICLEMANAGER_HPP

#include <MCParticle>
#include <MCTypes>
#include <MCVector3d>

#include <vector>
#include <memory>

//! ParticleManager takes care of spawning and recycling particles.
class ParticleManager
{
public:

    //! Constructor.
    ParticleManager();

    //! Destructor.
    ~ParticleManager();

    static ParticleManager & instance();

    void doSmoke(
        MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doSkidMark(
        MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doSparkle(
        MCVector3dFR location, MCVector3dFR velocity,
        MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doMud(
        MCVector3dFR location, MCVector3dFR velocity,
        MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

private:

    void preCreateParticles();

    // Free lists for different types of particles.
    mutable MCParticle::ParticleFreeList m_freeList;
    mutable MCParticle::ParticleFreeList m_freeList2;
    mutable MCParticle::ParticleFreeList m_freeList3;

    // Particles to delete.
    std::vector<std::shared_ptr<MCParticle> > m_delete;

    static ParticleManager * m_instance;
};

#endif // PARTICLEMANAGER_HPP
