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

#include "particlefactory.hpp"

#include "renderer.hpp"
#include "layers.hpp"

#include <MCAssetManager>
#include <MCGLColor>
#include <MCGLPointParticle>
#include <MCGLRectParticle>
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
        MCObject::getTypeIDForName(typeId), m_renderers[typeEnum]);
}

void ParticleFactory::preCreateRectParticles(int count,
    std::string typeId, ParticleFactory::ParticleType typeEnum)
{
    for (int i = 0; i < count; i++)
    {
        MCGLRectParticle * particle = new MCGLRectParticle(typeId);
        particle->setFreeList(m_freeLists[typeEnum]);
        particle->setShaderProgram(&Renderer::instance().program("particle"));

        // Initially push to list of free particles
        m_freeLists[typeEnum].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::unique_ptr<MCParticle>(particle));
    }
}

int scalePointSizeWithResolution(int size)
{
    return Renderer::instance().hRes() * size / 1600 + 1;
}

void ParticleFactory::preCreateParticles()
{
    preCreateRectParticles(500, "MUD", Mud);

    preCreatePointParticles(500, "ONSKID", OnTrackSkidMark, MCGLColor(0.1, 0.1, 0.1, 0.95));
    m_renderers[OnTrackSkidMark].setShaderProgram(&Renderer::instance().program("pointParticleRotate"));
    m_renderers[OnTrackSkidMark].setMaterial(MCAssetManager::surfaceManager().surface("skid").material());
    m_renderers[OnTrackSkidMark].setPointSize(scalePointSizeWithResolution(32));
    m_renderers[OnTrackSkidMark].setAlphaBlend(true);

    preCreatePointParticles(500, "OFFSKID", OffTrackSkidMark, MCGLColor(0.3, 0.2, 0.0, 0.95));
    m_renderers[OffTrackSkidMark].setShaderProgram(&Renderer::instance().program("pointParticleRotate"));
    m_renderers[OffTrackSkidMark].setMaterial(MCAssetManager::surfaceManager().surface("skid").material());
    m_renderers[OffTrackSkidMark].setPointSize(scalePointSizeWithResolution(32));
    m_renderers[OffTrackSkidMark].setAlphaBlend(true);

    preCreatePointParticles(500, "SMOKE", Smoke, MCGLColor(0.75, 0.75, 0.75, 0.75));
    m_renderers[Smoke].setShaderProgram(&Renderer::instance().program("pointParticle"));
    m_renderers[Smoke].setMaterial(MCAssetManager::surfaceManager().surface("smoke").material());
    m_renderers[Smoke].setPointSize(scalePointSizeWithResolution(32));
    m_renderers[Smoke].setAlphaBlend(true);

    preCreatePointParticles(500, "OFFSMOKE", OffTrackSmoke, MCGLColor(0.6, 0.4, 0.0, 0.5));
    m_renderers[OffTrackSmoke].setShaderProgram(&Renderer::instance().program("pointParticle"));
    m_renderers[OffTrackSmoke].setMaterial(MCAssetManager::surfaceManager().surface("smoke").material());
    m_renderers[OffTrackSmoke].setPointSize(scalePointSizeWithResolution(48));
    m_renderers[OffTrackSmoke].setAlphaBlend(true);

    preCreatePointParticles(500, "SPARKLE", Sparkle, MCGLColor(1.0, 0.75, 0.0, 1.0));
    m_renderers[Sparkle].setShaderProgram(&Renderer::instance().program("pointParticle"));
    m_renderers[Sparkle].setMaterial(MCAssetManager::surfaceManager().surface("sparkle").material());
    m_renderers[Sparkle].setPointSize(scalePointSizeWithResolution(16));
    m_renderers[Sparkle].setAlphaBlend(true);

    // Pre-create some leaf particles
    for (int i = 0; i < 100; i++)
    {
        MCSurfaceParticle * particle = new MCSurfaceParticle("LEAF");
        particle->setSurface(MCAssetManager::surfaceManager().surface("leaf"));
        particle->setShaderProgram(&Renderer::instance().program("default"));
        particle->setLayer(Layers::Tree);
        particle->setFreeList(m_freeLists[Leaf]);

        // Initially push to list of free particles
        m_freeLists[Leaf].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::unique_ptr<MCParticle>(particle));
    }
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

void ParticleFactory::doSmoke(MCVector3dFR location, MCVector3dFR velocity) const
{
    MCGLPointParticle * smoke = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[Smoke];
    if (freeList.size())
    {
        smoke = static_cast<MCGLPointParticle *>(freeList.back());
        freeList.pop_back();

        smoke->init(location, 10, 180);
        smoke->setAnimationStyle(MCParticle::FadeOutAndExpand);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->setVelocity(MCVector2dF(velocity) + MCRandom::randomVector2d() * 0.1);
        smoke->addToWorld();
    }
}

void ParticleFactory::doOffTrackSmoke(MCVector3dFR location) const
{
    MCGLPointParticle * smoke = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[OffTrackSmoke];
    if (freeList.size())
    {
        smoke = static_cast<MCGLPointParticle *>(freeList.back());
        freeList.pop_back();

        smoke->init(location, 10, 180);
        smoke->setAnimationStyle(MCParticle::FadeOut);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->setVelocity(MCRandom::randomVector2d() * 0.1);
        smoke->addToWorld();
    }
}

void ParticleFactory::doOnTrackSkidMark(MCVector3dFR location, int angle) const
{
    MCGLPointParticle * skidMark = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[OnTrackSkidMark];
    if (freeList.size())
    {
        skidMark = static_cast<MCGLPointParticle *>(freeList.back());
        freeList.pop_back();

        skidMark->init(location, 4, 1000);
        skidMark->setAnimationStyle(MCParticle::FadeOut);
        skidMark->rotate(angle);
        skidMark->setVelocity(MCVector3dF(0, 0, 0));
        skidMark->setAcceleration(MCVector3dF(0, 0, 0));
        skidMark->addToWorld();
    }
}

void ParticleFactory::doOffTrackSkidMark(MCVector3dFR location, int angle) const
{
    MCGLPointParticle * skidMark = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[OffTrackSkidMark];
    if (freeList.size())
    {
        skidMark = static_cast<MCGLPointParticle *>(freeList.back());
        freeList.pop_back();

        skidMark->init(location, 4, 1000);
        skidMark->setAnimationStyle(MCParticle::FadeOut);
        skidMark->rotate(angle);
        skidMark->setVelocity(MCVector3dF(0, 0, 0));
        skidMark->setAcceleration(MCVector3dF(0, 0, 0));
        skidMark->addToWorld();
    }
}

void ParticleFactory::doMud(MCVector3dFR location, MCVector3dFR velocity) const
{
    MCGLRectParticle * mud = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[Mud];
    if (freeList.size())
    {
        mud = static_cast<MCGLRectParticle *>(freeList.back());
        freeList.pop_back();

        mud->init(location, 4, 120);
        mud->setColor(MCGLColor(0.2, 0.1, 0.0, 1.0));
        mud->setAnimationStyle(MCParticle::Shrink);
        mud->rotate(MCRandom::getValue() * 360);
        mud->setVelocity(velocity + MCVector3dF(0, 0, 4.0f));
        mud->setAcceleration(MCVector3dF(0, 0, -10.0f));
        mud->setLayer(Layers::Mud);
        mud->addToWorld();
    }
}

void ParticleFactory::doSparkle(MCVector3dFR location, MCVector3dFR velocity) const
{
    MCGLPointParticle * sparkle = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[Sparkle];
    if (freeList.size())
    {
        sparkle = static_cast<MCGLPointParticle *>(freeList.back());
        freeList.pop_back();

        sparkle->init(location, 6, 120);
        sparkle->setAnimationStyle(MCParticle::FadeOut);
        sparkle->setVelocity(velocity);
        sparkle->addToWorld();
    }
}

void ParticleFactory::doLeaf(MCVector3dFR location, MCVector3dFR velocity) const
{
    MCSurfaceParticle * leaf = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[Leaf];
    if (freeList.size())
    {
        leaf = static_cast<MCSurfaceParticle *>(freeList.back());
        freeList.pop_back();

        leaf->init(location, 10, 120);
        leaf->setAnimationStyle(MCParticle::Shrink);
        leaf->rotate(MCRandom::getValue() * 360);
        leaf->setColor(MCGLColor(0.0, 0.75, 0.0, 0.75));
        leaf->setVelocity(velocity + MCVector3dF(0, 0, 2.0f) + MCRandom::randomVector3d());
        leaf->setAcceleration(MCVector3dF(0, 0, -5.0f));
        leaf->addToWorld();
    }
}

ParticleFactory::~ParticleFactory()
{
    ParticleFactory::m_instance = nullptr;
}
