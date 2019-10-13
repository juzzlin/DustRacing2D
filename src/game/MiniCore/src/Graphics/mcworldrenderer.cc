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
#include "mcobject.hh"
#include "mcparticle.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"
#include "mcsurfaceparticle.hh"
#include "mcsurfaceparticlerenderer.hh"
#include "mcsurfaceparticlerendererlegacy.hh"
#include "mcsurfaceview.hh"

#include <algorithm>

#include <MCGLEW>

MCWorldRenderer::MCWorldRenderer()
  : m_surfaceParticleRenderer(nullptr)
{
}

MCGLScene & MCWorldRenderer::glScene()
{
    return m_glScene;
}

void MCWorldRenderer::buildObjectBatches(MCCamera * camera)
{
    m_defaultLayer.objectBatches()[camera].clear();
    auto & batchVector = m_defaultLayer.objectBatches()[camera];
    static std::vector<MCObject *> childStack;
    childStack.clear();
    for (auto && object : MCWorld::instance().objectGrid().getObjectsWithinBBox(camera->bbox()))
    {
        childStack.push_back(object);
        while (childStack.size())
        {
            auto parent = childStack.back();
            childStack.pop_back();

            if (parent->isRenderable() && parent->shape() && parent->shape()->view())
            {
                const int objectViewId = object->typeId() * 1024 + parent->shape()->view()->viewId();
                auto batchIter = std::find_if(batchVector.begin(), batchVector.end(), [&](const MCRenderLayer::ObjectBatch & batch) {
                    return batch.objectViewId == objectViewId;
                });
                if (batchIter != batchVector.end())
                {
                    auto & batch = (*batchIter);
                    batch.objects.push_back(parent);
                    batch.priority = std::max(parent->location().k(), batch.priority);
                }
                else
                {
                    MCRenderLayer::ObjectBatch batch;
                    batch.objectViewId = objectViewId;
                    batch.objects.push_back(parent);
                    batch.priority = parent->location().k();
                    batchVector.push_back(batch);
                }
            }

            for (auto child : parent->children())
            {
                childStack.push_back(child.get());
            }
        }
    }

    std::stable_sort(batchVector.begin(), batchVector.end(), [](const MCRenderLayer::ObjectBatch & l, const MCRenderLayer::ObjectBatch & r) {
        return l.priority < r.priority;
    });
}

void MCWorldRenderer::buildParticleBatches(MCCamera * camera)
{
    m_defaultLayer.particleBatches()[camera].clear();
    auto & batchVector = m_defaultLayer.particleBatches()[camera];
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
            auto batchIter = std::find_if(batchVector.begin(), batchVector.end(), [&](const MCRenderLayer::ObjectBatch & batch) {
                return batch.objectViewId == static_cast<int>(particle.typeId());
            });
            if (batchIter != batchVector.end())
            {
                auto & batch = (*batchIter);
                batch.objects.push_back(&particle);
                batch.priority = std::max(particle.location().k(), batch.priority);
            }
            else
            {
                MCRenderLayer::ObjectBatch batch;
                batch.objectViewId = particle.typeId();
                batch.objects.push_back(&particle);
                batch.priority = particle.location().k();
                batchVector.push_back(batch);
            }
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

    std::stable_sort(batchVector.begin(), batchVector.end(), [](const MCRenderLayer::ObjectBatch & l, const MCRenderLayer::ObjectBatch & r) {
        return l.priority < r.priority;
    });
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

    if (!m_surfaceParticleRenderer)
    {
        createSurfaceParticleRenderer();
    }

    buildObjectBatches(camera);

    buildParticleBatches(camera);
}

void MCWorldRenderer::render(MCCamera * camera, MCRenderGroup renderGroup)
{
    switch (renderGroup)
    {
    case MCRenderGroup::Objects:
        renderObjects(camera);
        break;
    case MCRenderGroup::ObjectShadows:
        renderObjectShadows(camera);
        break;
    case MCRenderGroup::Particles:
        renderParticles(camera);
        break;
    case MCRenderGroup::ParticleShadows:
        renderParticleShadows(camera);
        break;
    default:
        break;
    }
}

void MCWorldRenderer::renderObjects(MCCamera * camera)
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

    glDepthMask(GL_TRUE);
}

void MCWorldRenderer::renderParticles(MCCamera * camera)
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

    renderParticleBatches(camera, m_defaultLayer);

    glDepthMask(GL_TRUE);
}

void MCWorldRenderer::renderObjectBatches(MCCamera * camera, MCRenderLayer & layer)
{
    for (auto && batch : layer.objectBatches()[camera])
    {
        const size_t itemCountInBatch = batch.objects.size();
        if (itemCountInBatch > 0)
        {
            MCObject * object = batch.objects[0];
            std::shared_ptr<MCShapeView> view = object->shape()->view();

            view->bind();
            object->render(camera);

            for (size_t i = 1; i < itemCountInBatch - 1; i++)
            {
                batch.objects[i]->render(camera);
            }

            object = batch.objects[itemCountInBatch - 1];
            object->render(camera);
            view->release();
        }
    }
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
        if (batch.objects.size())
        {
            if (dynamic_cast<MCSurfaceParticle *>(batch.objects[0]))
            {
                m_surfaceParticleRenderer->setBatch(batch, camera);
                m_surfaceParticleRenderer->render();
            }
        }
    }
}

void MCWorldRenderer::renderObjectShadows(MCCamera * camera)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);

    renderObjectShadowBatches(camera, m_defaultLayer);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

void MCWorldRenderer::renderParticleShadows(MCCamera * camera)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);

    renderParticleShadowBatches(camera, m_defaultLayer);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

void MCWorldRenderer::renderObjectShadowBatches(MCCamera * camera, MCRenderLayer & layer)
{
    // Render batches
    for (auto && batch : layer.objectBatches()[camera])
    {
        const size_t itemCountInBatch = batch.objects.size();
        if (itemCountInBatch > 0)
        {
            MCObject * object = batch.objects[0];
            std::shared_ptr<MCShapeView> view = object->shape()->view();
            if (view && view->hasShadow())
            {
                view->bindShadow();
                object->renderShadow(camera);

                for (size_t i = 1; i < itemCountInBatch - 1; i++)
                {
                    batch.objects[i]->renderShadow(camera);
                }

                object = batch.objects[itemCountInBatch - 1];
                object->renderShadow(camera);
                view->releaseShadow();
            }
        }
    }
}

void MCWorldRenderer::renderParticleShadowBatches(MCCamera * camera, MCRenderLayer & layer)
{
    for (auto && batch : layer.particleBatches()[camera])
    {
        if (batch.objects.size())
        {
            // Currently support shadows only for surface particles.
            if (MCSurfaceParticle * particle = dynamic_cast<MCSurfaceParticle *>(batch.objects[0]))
            {
                if (particle->hasShadow())
                {
                    m_surfaceParticleRenderer->setBatch(batch, camera, true);
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

void MCWorldRenderer::addObject(MCObject & object)
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

void MCWorldRenderer::removeObject(MCObject & object)
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
    delete m_surfaceParticleRenderer;
}
