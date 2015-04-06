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
#include "mcglpointparticle.hh"
#include "mcglpointparticlerenderer.hh"
#include "mcglrectparticle.hh"
#include "mcglrectparticlerenderer.hh"
#include "mcsurfaceparticle.hh"
#include "mcsurfaceparticlerenderer.hh"
#include "mcobject.hh"
#include "mcparticle.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"

#include <algorithm>

#include <MCGLEW>

MCWorldRenderer::MCWorldRenderer()
{
}

MCGLScene & MCWorldRenderer::glScene()
{
    return m_glScene;
}

void MCWorldRenderer::registerParticleRenderer(MCUint typeId, MCParticleRendererPtr renderer)
{
    m_particleRenderers[typeId] = renderer;
}

void MCWorldRenderer::render(MCCamera * camera, const std::vector<int> & layers)
{
    renderBatches(camera, layers);
}

void MCWorldRenderer::buildBatches(MCCamera * camera)
{
    // In the case of Dust Racing 2D, it was faster to just loop through
    // all objects on all layers and perform visibility tests instead of
    // just fetching all "visible" objects from MCObjectGrid.

    // This code tests the visibility and sorts the objects with respect
    // to their view id's into "batches". MCWorld::render()
    // (and MCWorld::renderShadows()) then goes through these batches
    // and perform the actual rendering.

    // Grouping the objects like this reduces texture switches etc and increases
    // overall performance.

    auto layerIter = m_layers.begin();
    while (layerIter != m_layers.end())
    {
        MCRenderLayer & layer = layerIter->second;

        layer.objectBatches()[camera].clear();
        layer.particleBatches()[camera].clear();

        for (auto objectIter = layer.objectSet().begin(); objectIter != layer.objectSet().end(); objectIter++)
        {
            MCObject & object = **objectIter;
            if (object.isRenderable())
            {
                // Check if view is set and is visible
                if (object.shape())
                {
                    if (!object.isParticle())
                    {
                        if (object.shape()->view())
                        {
                            MCBBox<MCFloat> bbox(object.shape()->view()->bbox().toBBox());
                            bbox.translate(MCVector2dF(object.location()));
                            if (!camera || camera->isVisible(bbox))
                            {
                                layer.objectBatches()[camera][object.typeID()].push_back(&object);
                            }
                        }
                    }
                    else
                    {
                        if (camera)
                        {
                            if (camera->isVisible(object.bbox()))
                            {
                                layer.particleBatches()[camera][object.typeID()].push_back(&object);
                            }
                            else
                            {
                                // Optimization that kills non-visible particles.
                                MCParticle & particle = static_cast<MCParticle &>(object);
                                if (particle.dieWhenOffScreen())
                                {
                                    bool isVisibleInAnyCamera = false;
                                    for (MCCamera * visibilityCamera : m_visibilityCameras)
                                    {
                                        if (visibilityCamera != camera && visibilityCamera->isVisible(particle.bbox()))
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
                        else
                        {
                            layer.particleBatches()[camera][object.typeID()].push_back(&object);
                        }
                    }
                }
            }
        }

        layerIter++;
    }
}

void MCWorldRenderer::renderBatches(MCCamera * camera, const std::vector<int> & layers)
{
    // Render in the order of the layers. Depth test is
    // layer-specific.

    auto layerIter = m_layers.begin();
    while (layerIter != m_layers.end())
    {
        // Render only given layers or all
        if (!layers.size() || std::find(layers.begin(), layers.end(), layerIter->first) != layers.end())
        {
            MCRenderLayer & layer = layerIter->second;

            // The depth test is enabled/disabled separately on
            // each object layer.
            if (layer.depthTestEnabled())
            {
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }

            glDepthMask(layer.depthMaskEnabled());

            renderObjectBatches(camera, layer);
            renderParticleBatches(camera, layer);

            glDepthMask(GL_TRUE);
        }

        layerIter++;
    }
}

void MCWorldRenderer::renderObjectBatches(MCCamera * camera, MCRenderLayer & layer)
{
    auto iter = layer.objectBatches()[camera].begin();
    const auto end = layer.objectBatches()[camera].end();
    while (iter != end)
    {
        const int itemCountInBatch = static_cast<const int>(iter->second.size());
        if (itemCountInBatch > 0)
        {
            MCObject * object = iter->second[0];
            std::shared_ptr<MCShapeView> view = object->shape()->view();
            view->beginBatch();
            object->render(camera);

            for (int i = 1; i < itemCountInBatch - 1; i++)
            {
                iter->second[i]->render(camera);
            }

            object = iter->second[itemCountInBatch - 1];
            object->render(camera);

            view = object->shape()->view();
            view->endBatch();
        }

        iter++;
    }
}

void MCWorldRenderer::renderParticleBatches(MCCamera * camera, MCRenderLayer & layer)
{
    auto batchIter = layer.particleBatches()[camera].begin();
    const auto end = layer.particleBatches()[camera].end();
    while (batchIter != end)
    {
        if (batchIter->second.size())
        {
            if (MCParticle * particle = dynamic_cast<MCParticle *>(batchIter->second[0]))
            {
                auto rendererIter = m_particleRenderers.find(particle->typeID());
                assert(rendererIter != m_particleRenderers.end());
                auto renderer = rendererIter->second;
                renderer->setBatch(batchIter->second, camera);
                renderer->render();
            }
        }

        batchIter++;
    }
}

void MCWorldRenderer::renderShadows(MCCamera * camera, const std::vector<int> & layers)
{
    glEnable(GL_DEPTH_TEST);

    auto layerIter = m_layers.begin();
    while (layerIter != m_layers.end())
    {
        // Render only given layers or all
        if (!layers.size() || std::find(layers.begin(), layers.end(), layerIter->first) != layers.end())
        {
            MCRenderLayer & layer = layerIter->second;
            renderObjectShadowBatches(camera, layer);
            renderParticleShadowBatches(camera, layer);
        }

        layerIter++;
    }

    glDisable(GL_DEPTH_TEST);
}

void MCWorldRenderer::renderObjectShadowBatches(MCCamera * camera, MCRenderLayer & layer)
{
    // Render batches
    auto batchIter = layer.objectBatches()[camera].begin();
    const auto end = layer.objectBatches()[camera].end();
    while (batchIter != end)
    {
        const int itemCountInBatch = static_cast<const int>(batchIter->second.size());
        if (itemCountInBatch > 0)
        {
            MCObject * object = batchIter->second[0];
            std::shared_ptr<MCShapeView> view = object->shape()->view();
            if (view && view->hasShadow())
            {
                view->beginShadowBatch();
                object->renderShadow(camera);

                for (int i = 1; i < itemCountInBatch - 1; i++)
                {
                    batchIter->second[i]->renderShadow(camera);
                }

                object = batchIter->second[itemCountInBatch - 1];
                object->renderShadow(camera);

                view = object->shape()->view();
                view->endShadowBatch();
            }
        }

        batchIter++;
    }
}

void MCWorldRenderer::renderParticleShadowBatches(MCCamera * camera, MCRenderLayer & layer)
{
    auto batchIter = layer.particleBatches()[camera].begin();
    const auto end = layer.particleBatches()[camera].end();
    while (batchIter != end)
    {
        if (batchIter->second.size())
        {
            // Currently support shadows only for surface particles.
            if (MCSurfaceParticle * particle = dynamic_cast<MCSurfaceParticle *>(batchIter->second[0]))
            {
                auto rendererIter = m_particleRenderers.find(particle->typeID());
                assert(rendererIter != m_particleRenderers.end());
                auto renderer = rendererIter->second;
                if (renderer->hasShadow() && renderer->shadowShaderProgram())
                {
                    renderer->setBatch(batchIter->second, camera);
                    renderer->renderShadows();
                }
            }
        }

        batchIter++;
    }
}

void MCWorldRenderer::enableDepthTestOnLayer(int layer, bool enable)
{
    m_layers[layer].setDepthTestEnabled(enable);
}

void MCWorldRenderer::enableDepthMaskOnLayer(int layer, bool enable)
{
    m_layers[layer].setDepthMaskEnabled(enable);
}

void MCWorldRenderer::addToLayerMap(MCObject & object)
{
    m_layers[object.renderLayer()].objectSet().insert(&object);
}

void MCWorldRenderer::removeFromLayerMap(MCObject & object)
{
    m_layers[object.renderLayer()].objectSet().erase(&object);
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
    auto layerIter = m_layers.begin();
    while (layerIter != m_layers.end())
    {
        layerIter->second.clear();
        layerIter++;
    }
}
