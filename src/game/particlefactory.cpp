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
#include <MCGLPointParticle>
#include <MCGLRectParticle>
#include <MCParticle>
#include <MCRandom>
#include <MCSurfaceParticle>
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

void ParticleFactory::preCreatePointParticles(int count,
    std::string typeId, ParticleFactory::ParticleType typeEnum,
    const MCGLColor & color)
{
    for (int i = 0; i < count; i++)
    {
        MCGLPointParticle * particle = new MCGLPointParticle(typeId, color);
        particle->setFreeList(m_freeLists[typeEnum]);

        if (typeEnum == ParticleFactory::OnTrackSkidMark)
        {
            particle->setDieWhenOffScreen(false);
        }

        // Initially push to list of free particles
        m_freeLists[typeEnum].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::unique_ptr<MCParticle>(particle));
    }

    MCWorld::instance().renderer().registerPointParticleRenderer(
        MCObject::getTypeIDForName(typeId), m_pointParticleRenderers[typeEnum]);
}

void ParticleFactory::preCreateRectParticles(int count,
    std::string typeId, ParticleFactory::ParticleType typeEnum)
{
    for (int i = 0; i < count; i++)
    {
        MCGLRectParticle * particle = new MCGLRectParticle(typeId);
        particle->setFreeList(m_freeLists[typeEnum]);

        // Initially push to list of free particles
        m_freeLists[typeEnum].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::unique_ptr<MCParticle>(particle));
    }

    MCWorld::instance().renderer().registerRectParticleRenderer(
        MCObject::getTypeIDForName(typeId), m_rectParticleRenderers[typeEnum]);
}

void ParticleFactory::preCreateSurfaceParticles(int count,
    std::string typeId, ParticleFactory::ParticleType typeEnum)
{
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

    MCWorld::instance().renderer().registerSurfaceParticleRenderer(
        MCObject::getTypeIDForName(typeId), m_surfaceParticleRenderers[typeEnum]);
}

int scalePointSizeWithResolution(int size)
{
    const int referenceWidth = 1600;
    return Renderer::instance().resolution().width() * size / referenceWidth + 1;
}

void ParticleFactory::updatePointSizes()
{
    m_pointParticleRenderers[Smoke].setPointSize(scalePointSizeWithResolution(32));
    m_pointParticleRenderers[OffTrackSmoke].setPointSize(scalePointSizeWithResolution(48));
    m_pointParticleRenderers[Sparkle].setPointSize(scalePointSizeWithResolution(16));
}

void ParticleFactory::preCreateParticles()
{
    preCreateSurfaceParticles(500, "SMOKE", Smoke);
    m_surfaceParticleRenderers[Smoke].setShaderProgram(Renderer::instance().program("default"));
    m_surfaceParticleRenderers[Smoke].setMaterial(MCAssetManager::surfaceManager().surface("smoke").material());
    m_surfaceParticleRenderers[Smoke].setAlphaBlend(true);

    preCreateSurfaceParticles(500, "OFFSMOKE", OffTrackSmoke);
    m_surfaceParticleRenderers[OffTrackSmoke].setShaderProgram(Renderer::instance().program("default"));
    m_surfaceParticleRenderers[OffTrackSmoke].setMaterial(MCAssetManager::surfaceManager().surface("smoke").material());
    m_surfaceParticleRenderers[OffTrackSmoke].setAlphaBlend(true);

    preCreatePointParticles(500, "SPARKLE", Sparkle, MCGLColor(1.0f, 0.75f, 0.0f, 1.0f));
    m_pointParticleRenderers[Sparkle].setShaderProgram(Renderer::instance().program("pointParticle"));
    m_pointParticleRenderers[Sparkle].setMaterial(MCAssetManager::surfaceManager().surface("sparkle").material());
    m_pointParticleRenderers[Sparkle].setAlphaBlend(true);

    preCreateSurfaceParticles(100, "LEAF", Leaf);
    m_surfaceParticleRenderers[Leaf].setShaderProgram(Renderer::instance().program("default"));
    m_surfaceParticleRenderers[Leaf].setShadowShaderProgram(Renderer::instance().program("defaultShadow"));
    m_surfaceParticleRenderers[Leaf].setMaterial(MCAssetManager::surfaceManager().surface("leaf").material());
    m_surfaceParticleRenderers[Leaf].setAlphaBlend(false);
    m_surfaceParticleRenderers[Leaf].setHasShadow(true);

    preCreateSurfaceParticles(500, "MUD", Mud);
    m_surfaceParticleRenderers[Mud].setShaderProgram(Renderer::instance().program("default"));
    m_surfaceParticleRenderers[Mud].setShadowShaderProgram(Renderer::instance().program("defaultShadow"));
    m_surfaceParticleRenderers[Mud].setMaterial(MCAssetManager::surfaceManager().surface("smoke").material());
    m_surfaceParticleRenderers[Mud].setAlphaBlend(false);
    m_surfaceParticleRenderers[Mud].setHasShadow(true);

    preCreateSurfaceParticles(500, "ONSKID", OnTrackSkidMark);
    m_surfaceParticleRenderers[OnTrackSkidMark].setShaderProgram(Renderer::instance().program("default"));
    m_surfaceParticleRenderers[OnTrackSkidMark].setMaterial(MCAssetManager::surfaceManager().surface("skid").material());
    m_surfaceParticleRenderers[OnTrackSkidMark].setAlphaBlend(true);

    preCreateSurfaceParticles(500, "OFFSKID", OffTrackSkidMark);
    m_surfaceParticleRenderers[OffTrackSkidMark].setShaderProgram(Renderer::instance().program("default"));
    m_surfaceParticleRenderers[OffTrackSkidMark].setMaterial(MCAssetManager::surfaceManager().surface("skid").material());
    m_surfaceParticleRenderers[OffTrackSkidMark].setAlphaBlend(true);

    updatePointSizes();
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

MCGLPointParticle * ParticleFactory::newPointParticle(ParticleType typeEnum) const
{
    MCGLPointParticle * p = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[typeEnum];
    if (freeList.size())
    {
        p = dynamic_cast<MCGLPointParticle *>(freeList.back());
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
    if (MCGLPointParticle * sparkle = ParticleFactory::newPointParticle(Sparkle))
    {
        sparkle->init(location, 6, 120);
        sparkle->setAnimationStyle(MCParticle::FadeOut);
        sparkle->setVelocity(velocity);
        sparkle->setRenderLayer(static_cast<int>(Layers::Render::Smoke));
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
