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

#include "particlefactory.hpp"

#include "renderer.hpp"
#include "layers.hpp"

#include <MCAssetManager>
#include <MCGLColor>
#include <MCParticle>
#include <MCPhysicsComponent>
#include <MCRandom>
#include <MCSurfaceParticle>
#include <MCSurfaceParticleRenderer>
#include <MCWorld>
#include <MCWorldRenderer>

#include <cassert>

ParticleFactory * ParticleFactory::m_instance = nullptr;

ParticleFactory::ParticleFactory()
{
    assert(!ParticleFactory::m_instance);
    ParticleFactory::m_instance = this;
    preCreateParticles();
}

ParticleFactory & ParticleFactory::instance()
{
    assert(ParticleFactory::m_instance);
    return *ParticleFactory::m_instance;
}

void ParticleFactory::preCreateSurfaceParticles(
    int count, std::string typeId, ParticleFactory::ParticleType typeEnum, MCSurface & surface, bool alphaBlend, bool hasShadow)
{
    m_particleRenderers[typeEnum] = MCParticleRendererPtr(new MCSurfaceParticleRenderer);

    for (int i = 0; i < count; i++)
    {
        MCSurfaceParticle * particle = new MCSurfaceParticle(typeId, surface);
        particle->setFreeList(m_freeLists[typeEnum]);
        particle->setCustomDeathCondition([] (MCParticle & self) {
            return self.location().k() <= 0;
        });
        particle->setAlphaBlend(alphaBlend);
        particle->setHasShadow(hasShadow);

        // Initially push to list of free particles
        m_freeLists[typeEnum].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::unique_ptr<MCParticle>(particle));
    }
}

void ParticleFactory::preCreateParticles()
{
    preCreateSurfaceParticles(500, "SMOKE", Smoke, MCAssetManager::surfaceManager().surface("smoke"), true);

    preCreateSurfaceParticles(500, "OFFSMOKE", OffTrackSmoke, MCAssetManager::surfaceManager().surface("smoke"), true);

    preCreateSurfaceParticles(500, "SPARKLE", Sparkle, MCAssetManager::surfaceManager().surface("sparkle"), true);

    preCreateSurfaceParticles(100, "LEAF", Leaf, MCAssetManager::surfaceManager().surface("leaf"), false, true);

    preCreateSurfaceParticles(500, "MUD", Mud, MCAssetManager::surfaceManager().surface("smoke"), false, true);

    preCreateSurfaceParticles(500, "ONSKID", OnTrackSkidMark, MCAssetManager::surfaceManager().surface("skid"), true);

    preCreateSurfaceParticles(500, "OFFSKID", OffTrackSkidMark, MCAssetManager::surfaceManager().surface("skid"), true);
}

void ParticleFactory::doParticle(
    ParticleType type, MCVector3dFR location, MCVector3dFR initialVelocity, int angle)
{
    switch (type)
    {
    case DamageSmoke:
        doDamageSmoke(location, initialVelocity);
        break;

    case Smoke:
        doSmoke(location, initialVelocity);
        break;

    case OffTrackSmoke:
        doOffTrackSmoke(location);
        break;

    case OnTrackSkidMark:
        doOnTrackSkidMark(location, angle);
        break;

    case OffTrackSkidMark:
        doOffTrackSkidMark(location, angle);
        break;

    case Sparkle:
        doSparkle(location, initialVelocity);
        break;

    case Mud:
        doMud(location, initialVelocity);
        break;

    case Leaf:
        doLeaf(location, initialVelocity);
        break;

    default:
        break;
    };
}

MCSurfaceParticle * ParticleFactory::newSurfaceParticle(ParticleType typeEnum) const
{
    MCSurfaceParticle * p = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[typeEnum];
    if (freeList.size())
    {
        p = dynamic_cast<MCSurfaceParticle *>(freeList.back());
        assert(p);
        freeList.pop_back();
    }

    return p;
}

void ParticleFactory::doDamageSmoke(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * smoke = ParticleFactory::newSurfaceParticle(Smoke))
    {
        smoke->init(location + MCVector3dF(0, 0, 10), 10, 180);
        smoke->setColor(MCGLColor(0.1f, 0.1f, 0.1f, 0.75f));
        smoke->setAnimationStyle(MCParticle::AnimationStyle::FadeOutAndExpand);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->physicsComponent().setVelocity(velocity + MCRandom::randomVector3dPositiveZ() * 0.2f);
        smoke->addToWorld();
    }
}

void ParticleFactory::doSmoke(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * smoke = ParticleFactory::newSurfaceParticle(Smoke))
    {
        smoke->init(location + MCVector3dF(0, 0, 10), 10, 180);
        smoke->setColor(MCGLColor(0.75f, 0.75f, 0.75f, 0.5f));
        smoke->setAnimationStyle(MCParticle::AnimationStyle::FadeOutAndExpand);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->physicsComponent().setVelocity(velocity + MCRandom::randomVector3dPositiveZ() * 0.1f);
        smoke->addToWorld();
    }
}

void ParticleFactory::doOffTrackSmoke(MCVector3dFR location) const
{
    if (MCSurfaceParticle * smoke = ParticleFactory::newSurfaceParticle(OffTrackSmoke))
    {
        smoke->init(location + MCVector3dF(0, 0, 10), 10, 180);
        smoke->setColor(MCGLColor(0.6f, 0.4f, 0.0f, 0.5f));
        smoke->setAnimationStyle(MCParticle::AnimationStyle::FadeOut);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->physicsComponent().setVelocity(MCRandom::randomVector3dPositiveZ() * 0.1f);
        smoke->addToWorld();
    }
}

void ParticleFactory::doOnTrackSkidMark(MCVector3dFR location, int angle) const
{
    if (MCSurfaceParticle * skidMark = newSurfaceParticle(OnTrackSkidMark))
    {
        skidMark->init(location + MCVector3dF(0, 0, 1), 8, 1000);
        skidMark->setColor(MCGLColor(0.1f, 0.1f, 0.1f, 1.0f));
        skidMark->setAnimationStyle(MCParticle::AnimationStyle::FadeOut);
        skidMark->rotate(angle);
        skidMark->physicsComponent().setVelocity(MCVector3dF(0, 0, 0));
        skidMark->physicsComponent().setAcceleration(MCVector3dF(0, 0, 0));
        skidMark->addToWorld();
    }
}

void ParticleFactory::doOffTrackSkidMark(MCVector3dFR location, int angle) const
{
    if (MCSurfaceParticle * skidMark = newSurfaceParticle(OffTrackSkidMark))
    {
        skidMark->init(location + MCVector3dF(0, 0, 1), 8, 1000);
        skidMark->setColor(MCGLColor(0.2f, 0.1f, 0.0f, 1.0f));
        skidMark->setAnimationStyle(MCParticle::AnimationStyle::FadeOut);
        skidMark->rotate(angle);
        skidMark->physicsComponent().setVelocity(MCVector3dF(0, 0, 0));
        skidMark->physicsComponent().setAcceleration(MCVector3dF(0, 0, 0));
        skidMark->addToWorld();
    }
}

void ParticleFactory::doMud(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * mud = newSurfaceParticle(Mud))
    {
        mud->init(location, 4, 180);
        mud->setColor(MCGLColor(0.2f, 0.1f, 0.0f, 1.0f));
        mud->setAnimationStyle(MCParticle::AnimationStyle::Shrink);
        mud->physicsComponent().setVelocity(velocity + MCVector3dF(0, 0, 4.0f));
        mud->physicsComponent().setAcceleration(MCWorld::instance().gravity());
        mud->addToWorld();
    }
}

void ParticleFactory::doSparkle(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * sparkle = ParticleFactory::newSurfaceParticle(Sparkle))
    {
        sparkle->init(location, 6, 120);
        sparkle->setColor(MCGLColor(1.0f, 0.75f, 0.0f, 1.0f));
        sparkle->setAnimationStyle(MCParticle::AnimationStyle::FadeOut);
        sparkle->physicsComponent().setVelocity(velocity + MCVector3dF(0, 0, 4.0f));
        sparkle->physicsComponent().setAcceleration(MCWorld::instance().gravity() * 0.5f);
        sparkle->addToWorld();
    }
}

void ParticleFactory::doLeaf(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * leaf = newSurfaceParticle(Leaf))
    {
        leaf->init(location, 10, 360);
        leaf->setAnimationStyle(MCParticle::AnimationStyle::Shrink);
        leaf->rotate(MCRandom::getValue() * 360);
        leaf->setColor(MCGLColor(0.0, 0.75f, 0.0, 0.75f));
        leaf->physicsComponent().setVelocity(velocity + MCVector3dF(0, 0, 2.0f) + MCRandom::randomVector3d());
        leaf->physicsComponent().setAngularVelocity((MCRandom::getValue() - 0.5) * 10.0f);
        leaf->physicsComponent().setMomentOfInertia(1.0f);
        leaf->physicsComponent().setAcceleration(MCVector3dF(0, 0, -2.5f));
        leaf->addToWorld();
    }
}

ParticleFactory::~ParticleFactory()
{
    ParticleFactory::m_instance = nullptr;
}
