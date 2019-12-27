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

#ifndef MCSURFACEPARTICLERENDERER_HH
#define MCSURFACEPARTICLERENDERER_HH

#include <MCGLEW>

#include "mcmacros.hh"
#include "mcparticlerendererbase.hh"
#include "mcworldrenderer.hh"

#include <vector>

class MCSurfaceParticle;
class MCCamera;
class MCObject;

/*! Renders surface particle (textured particles) batches.
 *  Each MCSurfaceParticle id should have a corresponding MCSurfaceParticleRenderer
 *  registered to MCWorldRenderer. */
class MCSurfaceParticleRenderer : public MCParticleRendererBase
{
public:
    explicit MCSurfaceParticleRenderer(size_t maxBatchSize = 1024);

    //! Destructor.
    virtual ~MCSurfaceParticleRenderer() override = default;

private:
    DISABLE_COPY(MCSurfaceParticleRenderer);
    DISABLE_ASSI(MCSurfaceParticleRenderer);
    DISABLE_MOVE(MCSurfaceParticleRenderer);

    /*! Populate the current batch.
     *  \param particles The vector of particle data to be rendered.
     *  \param camera The camera window. */
    void setBatch(MCRenderLayer::ObjectBatch & batch, MCCamera * camera = nullptr, bool isShadow = false) override;

    //! Render the current particle batch.
    void render() override;

    //! Render the current particle batch as shadows.
    void renderShadows() override;

    static const size_t m_numVerticesPerParticle;

    std::vector<MCGLVertex> m_vertices;

    std::vector<MCGLVertex> m_normals;

    std::vector<MCGLTexCoord> m_texCoords;

    std::vector<MCGLColor> m_colors;

    friend class MCWorldRenderer;
};

#endif // MCSURFACEPARTICLERENDERER_HH
