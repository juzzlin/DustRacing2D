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
#include "mcglvertex.hh"
#include "mcglcolor.hh"
#include "mcglobjectbase.hh"
#include "mcworldrenderer.hh"

#include <vector>

class MCGLRectParticle;
class MCGLShaderProgram;
class MCCamera;
class MCObject;

/*! Each MCGLRectParticle id should have a corresponding MCGLRectParticleRenderer
 *  registered to MCWorldRenderer. As for rendering, rect particles are special cases, because
 *  they need to be as efficient as possible. This is why a dedicated renderer is needed. */
class MCGLRectParticleRenderer : public MCGLObjectBase
{
public:

    explicit MCGLRectParticleRenderer(int maxBatchSize = 1024);

    //! Destructor.
    virtual ~MCGLRectParticleRenderer();

    //! Enable/disable blending.
    virtual void setAlphaBlend(
        bool useAlphaBlend, GLenum src = GL_SRC_ALPHA, GLenum dst = GL_ONE_MINUS_SRC_ALPHA);

private:

    DISABLE_COPY(MCGLRectParticleRenderer);
    DISABLE_ASSI(MCGLRectParticleRenderer);

    /*! Populate the current batch.
     *  \param particles The vector of particle data to be rendered.
     *  \param camera The camera window. */
    typedef std::vector<MCObject *> ParticleVector;
    virtual void setBatch(const ParticleVector & particles, MCCamera * camera = nullptr);

    //! Render the current particle batch.
    virtual void render();

    int          m_batchSize;
    int          m_maxBatchSize;
    MCGLVertex * m_vertices;
    MCGLVertex * m_normals;
    MCGLColor  * m_colors;
    bool         m_useAlphaBlend;
    GLenum       m_src;
    GLenum       m_dst;

    friend class MCWorldRenderer;
};

#endif // MCGLRECTPARTICLERENDERER_HH
