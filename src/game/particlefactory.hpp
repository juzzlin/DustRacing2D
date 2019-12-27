// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef PARTICLEFACTORY_HPP
#define PARTICLEFACTORY_HPP

#include <MCParticle>
#include <MCParticleRendererBase>

#include <MCVector3d>

#include <memory>
#include <vector>

class MCSurfaceParticle;

//! ParticleFactory takes care of spawning and recycling particles.
class ParticleFactory
{
public:
    enum ParticleType
    {
        DamageSmoke = 0,
        SkidSmoke,
        Smoke,
        OffTrackSmoke,
        OnTrackSkidMark,
        OffTrackSkidMark,
        Sparkle,
        Mud,
        Leaf,
        NumParticleTypes
    };

    //! Constructor.
    ParticleFactory();

    //! Destructor.
    ~ParticleFactory();

    static ParticleFactory & instance();

    void doParticle(
      ParticleType type,
      MCVector3dFR location,
      MCVector3dFR initialVelocity = MCVector3dF(0, 0, 0),
      int angle = 0);

private:
    void doDamageSmoke(MCVector3dFR location, MCVector3dFR velocity) const;

    void doSkidSmoke(MCVector3dFR location, MCVector3dFR velocity) const;

    void doSmoke(MCVector3dFR location, MCVector3dFR velocity) const;

    void doOffTrackSmoke(MCVector3dFR location) const;

    void doOnTrackSkidMark(MCVector3dFR location, int angle) const;

    void doOffTrackSkidMark(MCVector3dFR location, int angle) const;

    void doSparkle(MCVector3dFR location, MCVector3dFR velocity) const;

    void doMud(MCVector3dFR location, MCVector3dFR velocity) const;

    void doLeaf(MCVector3dFR location, MCVector3dFR velocity) const;

    void preCreateParticles();

    void preCreateSurfaceParticles(size_t count, std::string typeId, ParticleType typeEnum, MCSurfacePtr surface, bool alphaBlend = false, bool hasShadow = false);

    MCSurfaceParticle * newSurfaceParticle(ParticleType typeEnum) const;

    // Free lists (recycling) for different types of particles.
    mutable MCParticle::ParticleFreeList m_freeLists[NumParticleTypes];

    // Renderers for different particle types.
    MCParticleRendererPtr m_particleRenderers[NumParticleTypes];

    // Particles to delete.
    std::vector<std::unique_ptr<MCParticle>> m_delete;

    static ParticleFactory * m_instance;
};

#endif // PARTICLEFACTORY_HPP
