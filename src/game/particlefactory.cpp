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
    float r, float g, float b, float a)
{
    for (int i = 0; i < count; i++)
    {
        MCGLPointParticle * particle = new MCGLPointParticle(typeId, MCGLColor(r, g, b, a));
        particle->setFreeList(m_freeLists[typeEnum]);

        // Initially push to list of free particles
        m_freeLists[typeEnum].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::shared_ptr<MCParticle>(particle));
    }

    MCWorld::instance().registerPointParticleRenderer(
        MCObject::getTypeIDForName(typeId), m_renderers[typeEnum]);
}

void ParticleFactory::preCreateParticles()
{
    preCreatePointParticles(500, "MUD", Mud, 1.0, 1.0, 1.0, 1.0);
    m_renderers[Mud].setShaderProgram(&Renderer::instance().program("pointParticleDiscard"));
    m_renderers[Mud].setTexture(MCAssetManager::surfaceManager().surface("mud").handle1());
    m_renderers[Mud].setPointSize(12);

    preCreatePointParticles(500, "SKI", SkidMark, 0.3, 0.2, 0.0, 0.5);
    m_renderers[SkidMark].setShaderProgram(&Renderer::instance().program("pointParticleDiscard"));
    m_renderers[SkidMark].setTexture(MCAssetManager::surfaceManager().surface("mud").handle1());
    m_renderers[SkidMark].setPointSize(1);

    preCreatePointParticles(500, "SMO", Smoke, 0.75, 0.75, 0.75, 0.75);
    m_renderers[Smoke].setShaderProgram(&Renderer::instance().program("pointParticle"));
    m_renderers[Smoke].setTexture(MCAssetManager::surfaceManager().surface("smoke").handle1());
    m_renderers[Smoke].setPointSize(32);
    m_renderers[Smoke].setAlphaBlend(true);

    preCreatePointParticles(500, "SPA", Sparkle, 1.0, 0.75, 0.0, 1.0);
    m_renderers[Sparkle].setShaderProgram(&Renderer::instance().program("pointParticle"));
    m_renderers[Sparkle].setTexture(MCAssetManager::surfaceManager().surface("sparkle").handle1());
    m_renderers[Sparkle].setPointSize(16);
    m_renderers[Sparkle].setAlphaBlend(true);

    // Pre-create some leaf particles
    for (int i = 0; i < 100; i++)
    {
        MCSurfaceParticle * particle = new MCSurfaceParticle("LEAF");
        particle->setSurface(MCAssetManager::surfaceManager().surface("leaf"));
        particle->setShaderProgram(&Renderer::instance().program("master"));
        particle->setLayer(Layers::Tree);
        particle->setFreeList(m_freeLists[Leaf]);

        // Initially push to list of free particles
        m_freeLists[Leaf].push_back(particle);

        // Store for deletion
        m_delete.push_back(std::shared_ptr<MCParticle>(particle));
    }
}

void ParticleFactory::doParticle(ParticleType type, MCVector3dFR location)
{
    MCVector3dF zeroVelocity(0, 0, 0);
    doParticle(type, location, zeroVelocity);
}

void ParticleFactory::doParticle(
    ParticleType type, MCVector3dFR location, MCVector3dFR initialVelocity)
{
    switch (type)
    {
    case Smoke:
        doSmoke(location);
        break;

    case SkidMark:
        doSkidMark(location);
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

void ParticleFactory::doSmoke(MCVector3dFR location) const
{
    MCGLPointParticle * smoke = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[Smoke];
    if (freeList.size())
    {
        smoke = static_cast<MCGLPointParticle *>(freeList.back());
        freeList.pop_back();

        smoke->init(location, 10, 60);
        smoke->setAnimationStyle(MCParticle::FadeOut);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->setVelocity(MCRandom::randomVector2d() * 0.1);
        smoke->addToWorld();
    }
}

void ParticleFactory::doSkidMark(MCVector3dFR location) const
{
    MCGLPointParticle * skidMark = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[SkidMark];
    if (freeList.size())
    {
        skidMark = static_cast<MCGLPointParticle *>(freeList.back());
        freeList.pop_back();

        skidMark->init(location, 4, 3000);
        skidMark->setAnimationStyle(MCParticle::FadeOut);
        skidMark->setVelocity(MCVector3dF(0, 0, 0));
        skidMark->setAcceleration(MCVector3dF(0, 0, 0));
        skidMark->addToWorld();
    }
}

void ParticleFactory::doMud(MCVector3dFR location, MCVector3dFR velocity) const
{
    MCGLPointParticle * mud = nullptr;
    MCParticle::ParticleFreeList & freeList = m_freeLists[Mud];
    if (freeList.size())
    {
        mud = static_cast<MCGLPointParticle *>(freeList.back());
        freeList.pop_back();

        mud->init(location, 4, 120);
        mud->setAnimationStyle(MCParticle::Shrink);
        mud->setVelocity(velocity + MCVector3dF(0, 0, 2.0f));
        mud->setAcceleration(MCVector3dF(0, 0, -10.0f));
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
        leaf->setColor(0.0, 0.75, 0.0, 0.75);
        leaf->setVelocity(velocity + MCVector3dF(0, 0, 2.0f) + MCRandom::randomVector3d());
        leaf->setAcceleration(MCVector3dF(0, 0, -5.0f));
        leaf->addToWorld();
    }
}

ParticleFactory::~ParticleFactory()
{
    ParticleFactory::m_instance = nullptr;
}
