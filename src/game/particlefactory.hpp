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

#ifndef PARTICLEFACTORY_HPP
#define PARTICLEFACTORY_HPP

#include <MCParticle>
#include <MCGLPointParticleRenderer>
#include <MCTypes>
#include <MCVector3d>

#include <vector>
#include <memory>

//! ParticleFactory takes care of spawning and recycling particles.
class ParticleFactory
{
public:

    enum ParticleType
    {
        Smoke = 0,
        SkidMark,
        Sparkle,
        Mud,
        Leaf,
        NumParticles
    };

    //! Constructor.
    ParticleFactory();

    //! Destructor.
    ~ParticleFactory();

    static ParticleFactory & instance();

    void doParticle(
        ParticleType type, MCVector3dFR location, MCVector3dFR initialVelocity);

    void doParticle(
        ParticleType type, MCVector3dFR location);

private:

    void doSmoke(MCVector3dFR location) const;

    void doSkidMark(MCVector3dFR location) const;

    void doSparkle(MCVector3dFR location, MCVector3dFR velocity) const;

    void doMud(MCVector3dFR location, MCVector3dFR velocity) const;

    void doLeaf(MCVector3dFR location, MCVector3dFR velocity) const;

    void preCreateParticles();

    void preCreatePointParticles(int count,
        std::string typeId, ParticleType typeEnum,
        float r, float g, float b, float a);

    // Free lists (recycling) for different types of particles.
    mutable MCParticle::ParticleFreeList m_freeLists[NumParticles];

    // Renderers for different particle types.
    MCGLPointParticleRenderer m_renderers[NumParticles];

    // Particles to delete.
    std::vector<std::shared_ptr<MCParticle> > m_delete;

    static ParticleFactory * m_instance;
};

#endif // PARTICLEFACTORY_HPP
