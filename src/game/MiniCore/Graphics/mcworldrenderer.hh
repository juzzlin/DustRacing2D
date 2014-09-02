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

    void addToLayerMap(MCObject & object);

    void removeFromLayerMap(MCObject & object);

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

    /*! Must be called before calls to render() or renderShadows() */
    void buildBatches(MCCamera * camera);

    void render(MCCamera * camera);

    void renderShadows(MCCamera * camera);

    void clear();

private:

    void renderBatches(MCCamera * camera = nullptr);

    void renderObjectBatches(MCCamera * camera, int layer);

    void renderParticleBatches(MCCamera * camera, int layer);

    typedef std::unordered_set<MCObject *> LayerSet;
    LayerSet m_layers[MCWorld::MaxLayers];

    typedef std::map<int, std::vector<MCObject *> > BatchMap;
    typedef std::map<MCCamera *, BatchMap[MCWorld::MaxLayers]> CameraBatchMap;

    CameraBatchMap m_objectBatches;
    CameraBatchMap m_particleBatches;

    bool m_depthTestEnabled[MCWorld::MaxLayers];

    std::vector<MCCamera *> m_visibilityCameras;

    typedef std::map<int, MCGLPointParticleRenderer *> ParticleRendererMap;
    ParticleRendererMap m_particleRenderers;
};

#endif // MCWORLDRENDERER_HH
