// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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
#include "mcobject.hh"
#include "mcparticle.hh"
#include "mcshape.hh"
#include "mcshapeview.hh"

#include <algorithm>

#include <MCGLEW>

MCWorldRenderer::MCWorldRenderer()
{
}

void MCWorldRenderer::registerPointParticleRenderer(MCUint typeId, MCGLPointParticleRenderer & renderer)
{
    m_particleRenderers[typeId] = &renderer;
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

    bool depthTest = false;
    auto layerIter = m_layers.begin();
    while (layerIter != m_layers.end())
    {
        // Render only given layers or all
        if (!layers.size() || std::find(layers.begin(), layers.end(), layerIter->first) != layers.end())
        {
            MCRenderLayer & layer = layerIter->second;

            // The depth test is enabled/disabled separately on
            // each object layer.
            if (layer.depthTestEnabled() && !depthTest)
            {
                glEnable(GL_DEPTH_TEST);
                depthTest = true;
            }
            else if (depthTest)
            {
                glDisable(GL_DEPTH_TEST);
                depthTest = false;
            }

            renderObjectBatches(camera, layer);
            renderParticleBatches(camera, layer);
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
    // Render particle batches
    auto batchIter = layer.particleBatches()[camera].begin();
    const auto end = layer.particleBatches()[camera].end();
    while (batchIter != end)
    {
        if (!batchIter->second.size())
        {
            continue;
        }

        // Check if the batch is of MCGLPointParticles
        if (MCGLPointParticle * particle = dynamic_cast<MCGLPointParticle *>(batchIter->second[0]))
        {
            auto rendererIter = m_particleRenderers.find(particle->typeID());
            assert(rendererIter != m_particleRenderers.end());
            MCGLPointParticleRenderer * renderer = rendererIter->second;
            renderer->setBatch(batchIter->second, camera);
            renderer->render();
        }
        // Generic particles
        else
        {
            const int itemCountInBatch = static_cast<const int>(batchIter->second.size());
            if (itemCountInBatch > 0)
            {
                static_cast<MCParticle *>(batchIter->second[0])->beginBatch();
                static_cast<MCParticle *>(batchIter->second[0])->render(camera);

                for (int i = 1; i < itemCountInBatch - 1; i++)
                {
                    static_cast<MCParticle *>(batchIter->second[i])->render(camera);
                }

                static_cast<MCParticle *>(batchIter->second[itemCountInBatch - 1])->render(camera);
                static_cast<MCParticle *>(batchIter->second[itemCountInBatch - 1])->endBatch();
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

        layerIter++;
    }

    glDisable(GL_DEPTH_TEST);
}

void MCWorldRenderer::enableDepthTestOnLayer(int layer, bool enable)
{
    m_layers[layer].setDepthTestEnabled(enable);
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
    m_layers.clear();
}
