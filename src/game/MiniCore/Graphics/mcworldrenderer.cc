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

#include <MCGLEW>

MCWorldRenderer::MCWorldRenderer()
{
    for (unsigned i = 0; i < MCWorld::MaxLayers; i++)
    {
        m_depthTestEnabled[i] = false;
    }
}

void MCWorldRenderer::registerPointParticleRenderer(MCUint typeId, MCGLPointParticleRenderer & renderer)
{
    m_particleRenderers[typeId] = &renderer;
}

void MCWorldRenderer::render(MCCamera * camera, bool enableShadows)
{
    buildBatches(camera);

    if (enableShadows)
    {
        renderShadows(camera);
    }

    renderBatches(camera);
}

void MCWorldRenderer::buildBatches(MCCamera * camera)
{
    // In the case of Dust Racing 2D, it was faster to just loop through
    // all objects on all layers and perform visibility tests instead of
    // just fetching all "visible" objects from MCObjectTree.

    // This code tests the visibility and sorts the objects with respect
    // to their view id's into "batches". MCWorld::render()
    // (and MCWorld::renderShadows()) then goes through these batches
    // and perform the actual rendering.

    // Grouping the objects like this reduces texture switches etc and increases
    // overall performance.

    for (MCUint i = 0; i < MCWorld::MaxLayers; i++)
    {
        m_objectBatches[i].clear();
        m_particleBatches[i].clear();

        const auto end = m_layers[i].end();
        for (auto objectIter = m_layers[i].begin(); objectIter != end; objectIter++)
        {
            MCObject & object = **objectIter;
            if (object.renderable())
            {
                // Check if view is set and is visible
                if (object.shape())
                {
                    if (!object.isParticle())
                    {
                        if (object.shape()->view())
                        {
                            MCBBox<MCFloat> bbox(object.shape()->view()->bbox());
                            bbox.translate(MCVector2dF(object.location()));
                            if (!camera || camera->isVisible(bbox))
                            {
                                m_objectBatches[i][object.typeID()].push_back(&object);
                            }
                        }
                    }
                    else
                    {
                        if (camera)
                        {
                            if (camera->isVisible(object.bbox()))
                            {
                                m_particleBatches[i][object.typeID()].push_back(&object);
                            }
                            else
                            {
                                // Optimization that kills non-visible particles.
                                bool isVisibleInAnyCamera = false;
                                for (MCCamera * visibilityCamera : m_visibilityCameras)
                                {
                                    if (visibilityCamera != camera && visibilityCamera->isVisible(object.bbox()))
                                    {
                                        isVisibleInAnyCamera = true;
                                        break;
                                    }
                                }

                                if (!isVisibleInAnyCamera)
                                {
                                    static_cast<MCParticle &>(object).die();
                                }
                            }
                        }
                        else
                        {
                            m_particleBatches[i][object.typeID()].push_back(&object);
                        }
                    }
                }
            }
        }
    }
}

void MCWorldRenderer::renderBatches(MCCamera * camera)
{
    // Render in the order of the layers. Depth test is
    // layer-specific.
    glPushAttrib(GL_ENABLE_BIT);

    for (MCUint layer = 0; layer < MCWorld::MaxLayers; layer++)
    {
        // The depth test is enabled/disabled separately on
        // each object layer.
        if (m_depthTestEnabled[layer])
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        renderObjectBatches(camera, layer);
        renderParticleBatches(camera, layer);
    }

    glPopAttrib();
}

void MCWorldRenderer::renderObjectBatches(MCCamera * camera, int layer)
{
    auto iter = m_objectBatches[layer].begin();
    const auto end = m_objectBatches[layer].end();
    while (iter != end)
    {
        const int itemCountInBatch = iter->second.size();
        if (itemCountInBatch > 0)
        {
            MCObject * object = iter->second[0];
            MCShapeView * view = object->shape()->view();
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

void MCWorldRenderer::renderParticleBatches(MCCamera * camera, int layer)
{
    // Render particle batches
    auto batchIter = m_particleBatches[layer].begin();
    const auto end = m_particleBatches[layer].end();
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
            const int itemCountInBatch = batchIter->second.size();
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

void MCWorldRenderer::renderShadows(MCCamera * camera)
{
    glPushAttrib(GL_ENABLE_BIT);

    for (MCUint i = 0; i < MCWorld::MaxLayers; i++)
    {
        glDisable(GL_DEPTH_TEST);

        // Render batches
        auto iter = m_objectBatches[i].begin();
        const auto end = m_objectBatches[i].end();
        while (iter != end)
        {
            const int itemCountInBatch = iter->second.size();
            if (itemCountInBatch > 0)
            {
                MCObject * object = iter->second[0];
                MCShapeView * view = object->shape()->view();
                if (view && view->hasShadow())
                {
                    view->beginShadowBatch();
                    object->renderShadow(camera);

                    for (int i = 1; i < itemCountInBatch - 1; i++)
                    {
                        iter->second[i]->renderShadow(camera);
                    }

                    object = iter->second[itemCountInBatch - 1];
                    object->renderShadow(camera);

                    view = object->shape()->view();
                    view->endShadowBatch();
                }
            }

            iter++;
        }
    }

    glPopAttrib();
}

void MCWorldRenderer::enableDepthTestOnLayer(MCUint layer, bool enable)
{
    if (layer < MCWorld::MaxLayers)
    {
        m_depthTestEnabled[layer] = enable;
    }
}

void MCWorldRenderer::addToLayerMap(MCObject & object)
{
    const MCUint layerIndex =
        object.layer() >= MCWorld::MaxLayers ? MCWorld::MaxLayers - 1 : object.layer();
    m_layers[layerIndex].insert(&object);
}

void MCWorldRenderer::removeFromLayerMap(MCObject & object)
{
    const MCUint layerIndex =
        object.layer() >= MCWorld::MaxLayers ? MCWorld::MaxLayers - 1 : object.layer();
    m_layers[layerIndex].erase(&object);
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
    for (unsigned int i = 0; i < MCWorld::MaxLayers; i++)
    {
        m_layers[i].clear();
    }
}
