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

#ifndef MCGLRECTPARTICLERENDERER_HH
#define MCGLRECTPARTICLERENDERER_HH

#include <MCGLEW>

#include "mcmacros.hh"
#include "mcparticlerendererbase.hh"
#include "mcworldrenderer.hh"

#include <vector>

class MCGLRectParticle;
class MCCamera;
class MCObject;

/*! Renders rect particle batches. Each MCGLRectParticle id should have a corresponding
 *  MCGLRectParticleRenderer registered to MCWorldRenderer. */
class MCGLRectParticleRenderer : public MCParticleRendererBase
{
public:

    explicit MCGLRectParticleRenderer(int maxBatchSize = 1024);

    //! Destructor.
    virtual ~MCGLRectParticleRenderer();

private:

    DISABLE_COPY(MCGLRectParticleRenderer);
    DISABLE_ASSI(MCGLRectParticleRenderer);

    /*! Populate the current batch.
     *  \param particles The vector of particle data to be rendered.
     *  \param camera The camera window. */
    void setBatch(ParticleVector & particles, MCCamera * camera = nullptr) override;

    //! Render the current particle batch.
    void render() override;

    void renderShadows() override {}

    MCGLVertex * m_vertices;

    MCGLVertex * m_normals;

    MCGLColor * m_colors;

    friend class MCWorldRenderer;
};

#endif // MCGLRECTPARTICLERENDERER_HH
