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

#ifndef MCGLPOINTPARTICLERENDERER_HH
#define MCGLPOINTPARTICLERENDERER_HH

#include <MCGLEW>

#include "mcmacros.hh"
#include "mcparticlerendererbase.hh"
#include "mcworldrenderer.hh"

#include <vector>

class MCGLPointParticle;
class MCCamera;
class MCObject;

/*! Renders point particle batches.
 *  Each MCGLPointParticle id should have a corresponding MCGLPointParticleRenderer
 *  registered to MCWorldRenderer. */
class MCGLPointParticleRenderer : public MCParticleRendererBase
{
public:

    //! Constructor.
    explicit MCGLPointParticleRenderer(int maxBatchSize = 1024);

    //! Destructor.
    virtual ~MCGLPointParticleRenderer();

    //! Set default size of the points.
    virtual void setPointSize(int pointSize);

private:

    DISABLE_COPY(MCGLPointParticleRenderer);
    DISABLE_ASSI(MCGLPointParticleRenderer);

    /*! Populate the current batch.
     *  \param particles The vector of particle data to be rendered.
     *  \param camera The camera window. */
    void setBatch(const ParticleVector & particles, MCCamera * camera = nullptr) override;

    //! Render the current particle batch.
    void render() override;

    void renderShadows() override {}

    MCGLVertex * m_vertices;

    MCGLVertex * m_normals;

    MCGLColor  * m_colors;

    int m_pointSize;

    friend class MCWorldRenderer;
};

#endif // MCGLPOINTPARTICLERENDERER_HH
