// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcworldrenderer.hh"

#include "mccamera.hh"
#include "mclogger.hh"
#include "mcsurfaceobjectrenderer.hh"
#include "mcsurfaceobjectrendererlegacy.hh"
#include "mcsurfaceparticle.hh"
#include "mcsurfaceparticlerenderer.hh"
#include "mcsurfaceparticlerendererlegacy.hh"
#include "mcobject.hh"
#include "mcparticle.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mcsurfaceview.hh"

#include <algorithm>

#include <MCGLEW>

MCWorldRenderer::MCWorldRenderer()
    : m_surfaceObjectRenderer(nullptr)
    , m_surfaceParticleRenderer(nullptr)
{
}

MCGLScene & MCWorldRenderer::glScene()
{
    return m_glScene;
}

void MCWorldRenderer::render(MCCamera * camera)
{
    renderBatches(camera);
}

void MCWorldRenderer::buildBatches(MCCamera * camera)
{
    // This code tests the visibility and sorts the objects with respect
    // to their view id's into "batches". MCWorld::render()
    // (and MCWorld::renderShadows()) then goes through these batches
    // and perform the actual rendering.

    // Grouping the objects like this reduces texture switches etc and increases
    // overall performance.

    if (!camera)
    {
        return;
    }

    m_defaultLayer.objectBatches()[camera].clear();
    std::vector<MCObject *> childStack;
    for (auto && object : MCWorld::instance().objectGrid().getObjectsWithinBBox(camera->bbox()))
    {
        childStack.push_back(object);
        while (childStack.size())
        {
            MCObject * parent = childStack.back();
            childStack.pop_back();

            if (parent->isRenderable() && parent->shape() && parent->shape()->view())
            {
                m_defaultLayer.objectBatches()[camera][parent->shape()->view()->viewId()].push_back(parent);
            }

            for (auto child : parent->children())
            {
                childStack.push_back(child.get());
            }
        }
    }

    m_defaultLayer.particleBatches()[camera].clear();
    for (auto && particleIter : m_particleSet)
    {
        MCParticle & particle = *particleIter;
        const MCBBoxF bbox(
            particle.location().i() - particle.radius(),
            particle.location().j() - particle.radius(),
            particle.location().i() + particle.radius(),
            particle.location().j() + particle.radius());

        if (camera->isVisible(bbox))
        {
            m_defaultLayer.particleBatches()[camera][particle.typeID()].push_back(&particle);
        }
        else
        {
            // Optimization that kills non-visible particles.
            if (particle.dieWhenOffScreen())
            {
                bool isVisibleInAnyCamera = false;
                for (MCCamera * visibilityCamera : m_visibilityCameras)
                {
                    if (visibilityCamera != camera && visibilityCamera->isVisible(bbox))
                    {
                        isVisibleInAnyCamera = true;
                        break;
                    }
                }

                if (!isVisibleInAnyCamera)
                {
                    particle.die();
                }
            }
        }
    }
}

void MCWorldRenderer::renderBatches(MCCamera * camera)
{
    if (m_defaultLayer.depthTestEnabled())
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthMask(m_defaultLayer.depthMaskEnabled());

    renderObjectBatches(camera, m_defaultLayer);
    renderParticleBatches(camera, m_defaultLayer);

    glDepthMask(GL_TRUE);
}

void MCWorldRenderer::renderObjectBatches(MCCamera * camera, MCRenderLayer & layer)
{
    for (auto && batch : layer.objectBatches()[camera])
    {
        const int itemCountInBatch = static_cast<const int>(batch.second.size());
        if (itemCountInBatch > 0)
        {
            MCObject * object = batch.second[0];
            std::shared_ptr<MCShapeView> view = object->shape()->view();

            if (dynamic_cast<MCSurfaceView *>(view.get()))
            {
                if (!m_surfaceObjectRenderer)
                {
                    createSurfaceObjectRenderer();
                }

                m_surfaceObjectRenderer->setBatch(batch.second, camera);
                m_surfaceObjectRenderer->render();
            }
            else
            {
                view->beginBatch();
                object->render(camera);

                for (int i = 1; i < itemCountInBatch - 1; i++)
                {
                    batch.second[i]->render(camera);
                }

                object = batch.second[itemCountInBatch - 1];
                object->render(camera);

                view = object->shape()->view();
                view->endBatch();
            }
        }
    }
}

void MCWorldRenderer::createSurfaceObjectRenderer()
{
#ifdef __MC_GLES__
    MCLogger().info() << "Object renderer using vertex arrays.";
    m_surfaceObjectRenderer = new MCSurfaceObjectRendererLegacy;
#else
    MCLogger().info() << "Object renderer using VAO.";
    m_surfaceObjectRenderer = new MCSurfaceObjectRenderer;
#endif
}

void MCWorldRenderer::createSurfaceParticleRenderer()
{
#ifdef __MC_GLES__
    MCLogger().info() << "Particle renderer using vertex arrays.";
    m_surfaceParticleRenderer = new MCSurfaceParticleRendererLegacy;
#else
    MCLogger().info() << "Particle renderer using VAO.";
    m_surfaceParticleRenderer = new MCSurfaceParticleRenderer;
#endif
}

void MCWorldRenderer::renderParticleBatches(MCCamera * camera, MCRenderLayer & layer)
{
    for (auto && batch : layer.particleBatches()[camera])
    {
        if (batch.second.size())
        {
            if (dynamic_cast<MCSurfaceParticle *>(batch.second[0]))
            {
                if (!m_surfaceParticleRenderer)
                {
                    createSurfaceParticleRenderer();
                }

                m_surfaceParticleRenderer->setBatch(batch.second, camera);
                m_surfaceParticleRenderer->render();
            }
        }
    }
}

void MCWorldRenderer::renderShadows(MCCamera * camera)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA);

    renderObjectShadowBatches(camera, m_defaultLayer);
    renderParticleShadowBatches(camera, m_defaultLayer);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

void MCWorldRenderer::renderObjectShadowBatches(MCCamera * camera, MCRenderLayer & layer)
{
    // Render batches
    for (auto && batch : layer.objectBatches()[camera])
    {
        const int itemCountInBatch = static_cast<const int>(batch.second.size());
        if (itemCountInBatch > 0)
        {
            MCObject * object = batch.second[0];
            std::shared_ptr<MCShapeView> view = object->shape()->view();
            if (view && view->hasShadow())
            {
                if (dynamic_cast<MCSurfaceView *>(view.get()))
                {
                    if (!m_surfaceObjectRenderer)
                    {
                        createSurfaceObjectRenderer();
                    }

                    m_surfaceObjectRenderer->setBatch(batch.second, camera, true);
                    m_surfaceObjectRenderer->renderShadows();
                }
                else
                {
                    view->beginShadowBatch();
                    object->renderShadow(camera);

                    for (int i = 1; i < itemCountInBatch - 1; i++)
                    {
                        batch.second[i]->renderShadow(camera);
                    }

                    object = batch.second[itemCountInBatch - 1];
                    object->renderShadow(camera);

                    view = object->shape()->view();
                    view->endShadowBatch();
                }
            }
        }
    }
}

void MCWorldRenderer::renderParticleShadowBatches(MCCamera * camera, MCRenderLayer & layer)
{
    for (auto && batch : layer.particleBatches()[camera])
    {
        if (batch.second.size())
        {
            // Currently support shadows only for surface particles.
            if (MCSurfaceParticle * particle = dynamic_cast<MCSurfaceParticle *>(batch.second[0]))
            {
                if (particle->hasShadow())
                {
                    if (!m_surfaceParticleRenderer)
                    {
                        createSurfaceParticleRenderer();
                    }

                    m_surfaceParticleRenderer->setBatch(batch.second, camera, true);
                    m_surfaceParticleRenderer->renderShadows();
                }
            }
        }
    }
}

void MCWorldRenderer::enableDepthTest(bool enable)
{
    m_defaultLayer.setDepthTestEnabled(enable);
}

void MCWorldRenderer::enableDepthMask(bool enable)
{
    m_defaultLayer.setDepthMaskEnabled(enable);
}

void MCWorldRenderer::add(MCObject & object)
{
    if (object.isParticle())
    {
        MCParticle * particle = static_cast<MCParticle *>(&object);
        if (particle->m_indexInRenderArray == -1)
        {
            particle->m_indexInRenderArray = m_particleSet.size();
            m_particleSet.push_back(static_cast<MCParticle *>(&object));
        }
    }
}

void MCWorldRenderer::remove(MCObject & object)
{
    if (object.isParticle())
    {
        MCParticle * particle = static_cast<MCParticle *>(&object);
        if (particle->m_indexInRenderArray >= 0 && m_particleSet.size())
        {
            m_particleSet.back()->m_indexInRenderArray = particle->m_indexInRenderArray;
            m_particleSet[particle->m_indexInRenderArray] = m_particleSet.back();
            m_particleSet.pop_back();
            particle->m_indexInRenderArray = -1;
        }
    }
}

void MCWorldRenderer::addParticleVisibilityCamera(MCCamera & camera)
{
    m_visibilityCameras.push_back(&camera);
}

void MCWorldRenderer::removeParticleVisibilityCameras()
{
    m_visibilityCameras.clear();
}

void MCWorldRenderer::clear()
{
    m_defaultLayer.clear();

    for (auto particle : m_particleSet)
    {
        particle->m_indexInRenderArray = -1;
    }
    m_particleSet.clear();
}

MCWorldRenderer::~MCWorldRenderer()
{
    delete m_surfaceObjectRenderer;
    delete m_surfaceParticleRenderer;
}
