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

#include <MCGLPointParticle>
#include <MCGLRectParticle>
#include <MCRandom>
#include <MCSurfaceManager>
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
    // Pre-create some MCGLRectParticles
    for (int i = 0; i < 1000; i++)
    {
        MCParticle * particle = new MCGLRectParticle("MUD");
        static_cast<MCGLRectParticle *>(particle)->setShaderProgram(
            &Renderer::instance().program("particle"));
        particle->setFreeList(m_freeList);

        // Initially push to list of free particles
        m_freeList.push_back(particle);

        // Store for deletion
        m_delete.push_back(std::shared_ptr<MCParticle>(particle));
    }

    // Pre-create some MCSurfaceParticles
    for (int i = 0; i < 500; i++)
    {
        MCParticle * particle = new MCSurfaceParticle("SMOKE");

        particle->setSurface(&MCSurfaceManager::instance().surface("smoke"));
        particle->surface()->setShaderProgram(&Renderer::instance().program("master"));
        particle->setFreeList(m_freeList2);

        // Initially push to list of free particles
        m_freeList2.push_back(particle);

        // Store for deletion
        m_delete.push_back(std::shared_ptr<MCParticle>(particle));
    }

    // Pre-create some MCGLPointParticles
    for (int i = 0; i < 1000; i++)
    {
        MCParticle * particle = new MCGLPointParticle("SPARKLE");
        static_cast<MCGLPointParticle *>(particle)->setShaderProgram(
            &Renderer::instance().program("pointParticle"));
        particle->setFreeList(m_freeList3);

        // Initially push to list of free particles
        m_freeList3.push_back(particle);

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

        smoke->init(location, 10, 120);
        smoke->setAnimationStyle(MCParticle::Shrink);
        smoke->translate(location);
        smoke->rotate(MCRandom::getValue() * 360);
        smoke->setVelocity(MCRandom::randomVector2d() * 0.1);
        smoke->surface()->setColor(r, g, b, a);
        smoke->surface()->setAlphaBlend(true);
        smoke->addToWorld();
    }
}

void ParticleManager::doSkidMark(MCVector3dFR, MCFloat, MCFloat, MCFloat, MCFloat) const
{
    // Must be drawn to an offscreen buffer. Not implemented.
}

void ParticleManager::doMud(
    MCVector3dFR location, MCVector3dFR velocity, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    MCGLRectParticle * mud = nullptr;
    if (m_freeList.size())
    {
        mud = static_cast<MCGLRectParticle *>(m_freeList.back());
        m_freeList.pop_back();

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
    MCGLPointParticle * sparkle = nullptr;
    if (m_freeList3.size())
    {
        sparkle = static_cast<MCGLPointParticle *>(m_freeList3.back());
        m_freeList3.pop_back();

        sparkle->init(location, 2, 60);
        sparkle->setAnimationStyle(MCParticle::Shrink);
        sparkle->setColor(r, g, b, a);
        sparkle->setVelocity(velocity);
        sparkle->addToWorld();
    }
}

ParticleManager::~ParticleManager()
{
    ParticleManager::m_instance = nullptr;
}
