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
#include <MCGLRectParticle>
#include <MCGLRectParticleRenderer>
#include <MCParticle>
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

void ParticleFactory::preCreateRectParticles(int count,
    std::string typeId, ParticleFactory::ParticleType typeEnum)
{
    m_particleRenderers[typeEnum] = MCParticleRendererPtr(new MCGLRectParticleRenderer);

    for (int i = 0; i < count; i++)
    {
        MCGLRectParticle * particle = new MCGLRectParticle(typeId);
        particle->setFreeList(m_freeLists[typeEnum]);

        // Initially push to list of free particles
        m_freeLists[typeEnum].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::unique_ptr<MCParticle>(particle));
    }

    MCWorld::instance().renderer().registerParticleRenderer(
        MCObject::getTypeIDForName(typeId), m_particleRenderers[typeEnum]);
}

void ParticleFactory::preCreateSurfaceParticles(int count,
    std::string typeId, ParticleFactory::ParticleType typeEnum)
{
    m_particleRenderers[typeEnum] = MCParticleRendererPtr(new MCSurfaceParticleRenderer);

    for (int i = 0; i < count; i++)
    {
        MCSurfaceParticle * particle = new MCSurfaceParticle(typeId);
        particle->setRenderLayer(static_cast<int>(Layers::Render::Objects));
        particle->setFreeList(m_freeLists[typeEnum]);
        particle->setCustomDeathCondition([] (MCParticle & self) {
            return self.location().k() <= 0;
        });

        // Initially push to list of free particles
        m_freeLists[typeEnum].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::unique_ptr<MCParticle>(particle));
    }

    MCWorld::instance().renderer().registerParticleRenderer(
        MCObject::getTypeIDForName(typeId), m_particleRenderers[typeEnum]);
}

void ParticleFactory::preCreateParticles()
{
    preCreateSurfaceParticles(500, "SMOKE", Smoke);
    m_particleRenderers[Smoke]->setShaderProgram(Renderer::instance().program("default"));
    m_particleRenderers[Smoke]->setMaterial(MCAssetManager::surfaceManager().surface("smoke").material());
    m_particleRenderers[Smoke]->setAlphaBlend(true);

    preCreateSurfaceParticles(500, "OFFSMOKE", OffTrackSmoke);
    m_particleRenderers[OffTrackSmoke]->setShaderProgram(Renderer::instance().program("default"));
    m_particleRenderers[OffTrackSmoke]->setMaterial(MCAssetManager::surfaceManager().surface("smoke").material());
    m_particleRenderers[OffTrackSmoke]->setAlphaBlend(true);

    preCreateSurfaceParticles(500, "SPARKLE", Sparkle);
    m_particleRenderers[Sparkle]->setShaderProgram(Renderer::instance().program("default"));
    m_particleRenderers[Sparkle]->setMaterial(MCAssetManager::surfaceManager().surface("sparkle").material());
    m_particleRenderers[Sparkle]->setAlphaBlend(true);

    preCreateSurfaceParticles(100, "LEAF", Leaf);
    m_particleRenderers[Leaf]->setShaderProgram(Renderer::instance().program("default"));
    m_particleRenderers[Leaf]->setShadowShaderProgram(Renderer::instance().program("defaultShadow"));
    m_particleRenderers[Leaf]->setMaterial(MCAssetManager::surfaceManager().surface("leaf").material());
    m_particleRenderers[Leaf]->setAlphaBlend(false);
    m_particleRenderers[Leaf]->setHasShadow(true);

    preCreateSurfaceParticles(500, "MUD", Mud);
    m_particleRenderers[Mud]->setShaderProgram(Renderer::instance().program("default"));
    m_particleRenderers[Mud]->setShadowShaderProgram(Renderer::instance().program("defaultShadow"));
    m_particleRenderers[Mud]->setMaterial(MCAssetManager::surfaceManager().surface("smoke").material());
    m_particleRenderers[Mud]->setAlphaBlend(false);
    m_particleRenderers[Mud]->setHasShadow(true);

    preCreateSurfaceParticles(500, "ONSKID", OnTrackSkidMark);
    m_particleRenderers[OnTrackSkidMark]->setShaderProgram(Renderer::instance().program("default"));
    m_particleRenderers[OnTrackSkidMark]->setMaterial(MCAssetManager::surfaceManager().surface("skid").material());
    m_particleRenderers[OnTrackSkidMark]->setAlphaBlend(true);

    preCreateSurfaceParticles(500, "OFFSKID", OffTrackSkidMark);
    m_particleRenderers[OffTrackSkidMark]->setShaderProgram(Renderer::instance().program("default"));
    m_particleRenderers[OffTrackSkidMark]->setMaterial(MCAssetManager::surfaceManager().surface("skid").material());
    m_particleRenderers[OffTrackSkidMark]->setAlphaBlend(true);
}

void ParticleFactory::doParticle(
    ParticleType type, MCVector3dFR location, MCVector3dFR initialVelocity, int angle)
{
    switch (type)
    {
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

void ParticleFactory::doSmoke(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * smoke = ParticleFactory::newSurfaceParticle(Smoke))
    {
        smoke->init(location + MCVector3dF(0, 0, 10), 10, 180);
        smoke->setColor(MCGLColor(0.75f, 0.75f, 0.75f, 0.5f));
        smoke->setAnimationStyle(MCParticle::FadeOutAndExpand);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->setVelocity(velocity + MCRandom::randomVector3dPositiveZ() * 0.1f);
        smoke->setRenderLayer(static_cast<int>(Layers::Render::Smoke));
        smoke->addToWorld();
    }
}

void ParticleFactory::doOffTrackSmoke(MCVector3dFR location) const
{
    if (MCSurfaceParticle * smoke = ParticleFactory::newSurfaceParticle(OffTrackSmoke))
    {
        smoke->init(location + MCVector3dF(0, 0, 10), 10, 180);
        smoke->setColor(MCGLColor(0.6f, 0.4f, 0.0f, 0.5f));
        smoke->setAnimationStyle(MCParticle::FadeOut);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->setVelocity(MCRandom::randomVector3dPositiveZ() * 0.1f);
        smoke->setRenderLayer(static_cast<int>(Layers::Render::Smoke));
        smoke->addToWorld();
    }
}

void ParticleFactory::doOnTrackSkidMark(MCVector3dFR location, int angle) const
{
    if (MCSurfaceParticle * skidMark = newSurfaceParticle(OnTrackSkidMark))
    {
        skidMark->init(location + MCVector3dF(0, 0, 1), 8, 1000);
        skidMark->setColor(MCGLColor(0.1f, 0.1f, 0.1f, 1.0f));
        skidMark->setAnimationStyle(MCParticle::FadeOut);
        skidMark->rotate(angle);
        skidMark->setVelocity(MCVector3dF(0, 0, 0));
        skidMark->setAcceleration(MCVector3dF(0, 0, 0));
        skidMark->setRenderLayer(static_cast<int>(Layers::Render::Ground));
        skidMark->addToWorld();
    }
}

void ParticleFactory::doOffTrackSkidMark(MCVector3dFR location, int angle) const
{
    if (MCSurfaceParticle * skidMark = newSurfaceParticle(OffTrackSkidMark))
    {
        skidMark->init(location + MCVector3dF(0, 0, 1), 8, 1000);
        skidMark->setColor(MCGLColor(0.2f, 0.1f, 0.0f, 1.0f));
        skidMark->setAnimationStyle(MCParticle::FadeOut);
        skidMark->rotate(angle);
        skidMark->setVelocity(MCVector3dF(0, 0, 0));
        skidMark->setAcceleration(MCVector3dF(0, 0, 0));
        skidMark->setRenderLayer(static_cast<int>(Layers::Render::Ground));
        skidMark->addToWorld();
    }
}

void ParticleFactory::doMud(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * mud = newSurfaceParticle(Mud))
    {
        mud->init(location, 4, 180);
        mud->setColor(MCGLColor(0.2f, 0.1f, 0.0f, 1.0f));
        mud->setAnimationStyle(MCParticle::Shrink);
        mud->setVelocity(velocity + MCVector3dF(0, 0, 4.0f));
        mud->setAcceleration(MCWorld::instance().gravity());
        mud->setRenderLayer(static_cast<int>(Layers::Render::Objects));
        mud->addToWorld();
    }
}

void ParticleFactory::doSparkle(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * sparkle = ParticleFactory::newSurfaceParticle(Sparkle))
    {
        sparkle->init(location, 6, 120);
        sparkle->setColor(MCGLColor(1.0f, 0.75f, 0.0f, 1.0f));
        sparkle->setAnimationStyle(MCParticle::FadeOut);
        sparkle->setVelocity(velocity + MCVector3dF(0, 0, 4.0f));
        sparkle->setAcceleration(MCWorld::instance().gravity() * 0.5f);
        sparkle->setRenderLayer(static_cast<int>(Layers::Render::Sparkles));
        sparkle->addToWorld();
    }
}

void ParticleFactory::doLeaf(MCVector3dFR location, MCVector3dFR velocity) const
{
    if (MCSurfaceParticle * leaf = newSurfaceParticle(Leaf))
    {
        leaf->init(location, 10, 360);
        leaf->setAnimationStyle(MCParticle::Shrink);
        leaf->rotate(MCRandom::getValue() * 360);
        leaf->setColor(MCGLColor(0.0, 0.75, 0.0, 0.75));
        leaf->setVelocity(velocity + MCVector3dF(0, 0, 2.0f) + MCRandom::randomVector3d());
        leaf->setAngularVelocity((MCRandom::getValue() - 0.5) * 10.0f);
        leaf->setMomentOfInertia(1.0);
        leaf->setAcceleration(MCVector3dF(0, 0, -2.5f));
        leaf->addToWorld();
    }
}

ParticleFactory::~ParticleFactory()
{
    ParticleFactory::m_instance = nullptr;
}
