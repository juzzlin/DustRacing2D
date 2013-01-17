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

#include "particlemanager.hpp"

#include "renderer.hpp"
#include "layers.hpp"

#include <MCAssetManager>
#include <MCGLPointParticle>
#include <MCGLRectParticle>
#include <MCRandom>
#include <MCSurfaceParticle>

#include <cassert>

ParticleManager * ParticleManager::m_instance = nullptr;

ParticleManager::ParticleManager()
{
    assert(!ParticleManager::m_instance);
    ParticleManager::m_instance = this;
    preCreateParticles();
}

ParticleManager & ParticleManager::instance()
{
    assert(ParticleManager::m_instance);
    return *ParticleManager::m_instance;
}

void ParticleManager::preCreateParticles()
{
    // Pre-create some mud particles
    for (int i = 0; i < 250; i++)
    {
        MCParticle * particle = new MCGLRectParticle("MUD");
        static_cast<MCGLRectParticle *>(particle)->setShaderProgram(
            &Renderer::instance().program("particle"));
        particle->setFreeList(m_freeList1);

        // Initially push to list of free particles
        m_freeList1.push_back(particle);

        // Store for deletion
        m_delete.push_back(std::shared_ptr<MCParticle>(particle));
    }

    // Pre-create some smoke particles
    for (int i = 0; i < 100; i++)
    {
        MCSurfaceParticle * particle = new MCSurfaceParticle("SMOKE");
        particle->setSurface(MCAssetManager::surfaceManager().surface("smoke"));
        particle->setShaderProgram(&Renderer::instance().program("master"));
        particle->setFreeList(m_freeList2);

        // Initially push to list of free particles
        m_freeList2.push_back(particle);

        // Store for deletion
        m_delete.push_back(std::shared_ptr<MCParticle>(particle));
    }

    // Pre-create some sparkles
    for (int i = 0; i < 100; i++)
    {
        MCSurfaceParticle * particle = new MCSurfaceParticle("SPARKLE");
        particle->setSurface(MCAssetManager::surfaceManager().surface("sparkle"));
        particle->setShaderProgram(&Renderer::instance().program("master"));
        particle->setFreeList(m_freeList3);

        // Initially push to list of free particles
        m_freeList3.push_back(particle);

        // Store for deletion
        m_delete.push_back(std::shared_ptr<MCParticle>(particle));
    }

    // Pre-create some leaf particles
    for (int i = 0; i < 100; i++)
    {
        MCSurfaceParticle * particle = new MCSurfaceParticle("LEAF");
        particle->setSurface(MCAssetManager::surfaceManager().surface("leaf"));
        particle->setShaderProgram(&Renderer::instance().program("master"));
        particle->setLayer(Layers::Tree);
        particle->setFreeList(m_freeList4);

        // Initially push to list of free particles
        m_freeList4.push_back(particle);

        // Store for deletion
        m_delete.push_back(std::shared_ptr<MCParticle>(particle));
    }
}

void ParticleManager::doSmoke(
    MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    MCSurfaceParticle * smoke = nullptr;
    if (m_freeList2.size())
    {
        smoke = static_cast<MCSurfaceParticle *>(m_freeList2.back());
        m_freeList2.pop_back();

        smoke->init(location, 10, 60);
        smoke->setAnimationStyle(MCParticle::Shrink);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->setVelocity(MCRandom::randomVector2d() * 0.1);
        smoke->setColor(r, g, b, a);
        smoke->addToWorld();
    }
}

void ParticleManager::doSkidMark(
    MCVector3dFR location, MCFloat angle, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    MCGLRectParticle * skidMark = nullptr;
    if (m_freeList1.size())
    {
        skidMark = static_cast<MCGLRectParticle *>(m_freeList1.back());
        m_freeList1.pop_back();

        skidMark->init(location, 4, 3000);
        skidMark->setAnimationStyle(MCParticle::FadeOut);
        skidMark->rotate(angle);
        skidMark->setColor(r, g, b, a);
        skidMark->setVelocity(MCVector3dF(0, 0, 0));
        skidMark->setAcceleration(MCVector3dF(0, 0, 0));
        skidMark->addToWorld();
    }
}

void ParticleManager::doMud(
    MCVector3dFR location, MCVector3dFR velocity, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    MCGLRectParticle * mud = nullptr;
    if (m_freeList1.size())
    {
        mud = static_cast<MCGLRectParticle *>(m_freeList1.back());
        m_freeList1.pop_back();

        mud->init(location, 4, 120);
        mud->setAnimationStyle(MCParticle::Shrink);
        mud->setColor(r, g, b, a);
        mud->setVelocity(velocity + MCVector3dF(0, 0, 2.0f));
        mud->setAcceleration(MCVector3dF(0, 0, -10.0f));
        mud->addToWorld();
    }
}

void ParticleManager::doSparkle(
    MCVector3dFR location, MCVector3dFR velocity, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    MCSurfaceParticle * sparkle = nullptr;
    if (m_freeList3.size())
    {
        sparkle = static_cast<MCSurfaceParticle *>(m_freeList3.back());
        m_freeList3.pop_back();

        sparkle->init(location, 6, 60);
        sparkle->setAnimationStyle(MCParticle::Shrink);
        sparkle->setColor(r, g, b, a);
        sparkle->setVelocity(velocity);
        sparkle->addToWorld();
    }
}

void ParticleManager::doLeaf(
    MCVector3dFR location, MCVector3dFR velocity, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    MCSurfaceParticle * leaf = nullptr;
    if (m_freeList4.size())
    {
        leaf = static_cast<MCSurfaceParticle *>(m_freeList4.back());
        m_freeList4.pop_back();

        leaf->init(location, 10, 120);
        leaf->setAnimationStyle(MCParticle::Shrink);
        leaf->rotate(MCRandom::getValue() * 360);
        leaf->setColor(r, g, b, a);
        leaf->setVelocity(velocity + MCVector3dF(0, 0, 2.0f) + MCRandom::randomVector3d());
        leaf->setAcceleration(MCVector3dF(0, 0, -5.0f));
        leaf->addToWorld();
    }
}

ParticleManager::~ParticleManager()
{
    ParticleManager::m_instance = nullptr;
}
