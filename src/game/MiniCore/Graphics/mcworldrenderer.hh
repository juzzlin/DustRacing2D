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

#ifndef MCWORLDRENDERER_HH
#define MCWORLDRENDERER_HH

#include "mcrenderlayer.hh"
#include "mctypes.hh"
#include "mcworld.hh"

#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

class MCCamera;
class MCGLPointParticleRenderer;
class MCObject;

//! Helper class used by MCWorld. Renders all objects in the scene.
class MCWorldRenderer
{
public:

    MCWorldRenderer();

    void enableDepthTestOnLayer(int layer, bool enable = true);

    /*! Each used MCGLPointParticle should have a corresponding MCGLPointParticleRenderer
     *  registered in MCWorld. As for rendering, point particles are special cases, because
     *  they need to be as efficient as possible. This is why a dedicated renderer is needed.
     *  \param typeId Type id of the point particle. \see MCGLPointParticle.
     *  \param renderer Reference to the renderer to be used for this type id. */
    void registerPointParticleRenderer(MCUint typeId, MCGLPointParticleRenderer & renderer);

    /*! If a particle gets off all visibility cameras, it'll be killed.
     *  This is just an optimization. We cannot use just the camera given
     *  to render(), because there might be multiple cameras and viewports. */
    void addParticleVisibilityCamera(MCCamera & camera);

    void removeParticleVisibilityCameras();

private:

    void addToLayerMap(MCObject & object);

    /*! Must be called before calls to render() or renderShadows() */
    void buildBatches(MCCamera * camera);

    void clear();

    void removeFromLayerMap(MCObject & object);

    void render(MCCamera * camera, const std::vector<int> & layers);

    void renderBatches(MCCamera * camera = nullptr, const std::vector<int> & layers = std::vector<int>());

    void renderObjectBatches(MCCamera * camera, MCRenderLayer & layer);

    void renderParticleBatches(MCCamera * camera, MCRenderLayer & layer);

    void renderShadows(MCCamera * camera, const std::vector<int> & layers);

    typedef int LayerId;
    std::map<LayerId, MCRenderLayer> m_layers;

    std::vector<MCCamera *> m_visibilityCameras;

    typedef int ParticleTypeId;
    typedef std::map<ParticleTypeId, MCGLPointParticleRenderer *> ParticleRendererMap;
    ParticleRendererMap m_particleRenderers;

    friend class MCWorld;
    friend class MCObject;
};

#endif // MCWORLDRENDERER_HH
