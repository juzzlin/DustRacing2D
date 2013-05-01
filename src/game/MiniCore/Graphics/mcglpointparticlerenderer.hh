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
#include "mcglvertex.hh"
#include "mcglcolor.hh"
#include "mcworldrenderer.hh"

#include <vector>

class MCGLPointParticle;
class MCGLShaderProgram;
class MCCamera;
class MCObject;

/*! Each MCGLPointParticle id should have a corresponding MCGLPointParticleRenderer
 *  registered to MCWorldRenderer. As for rendering, point particles are special cases, because
 *  they need to be as efficient as possible. This is why a dedicated renderer is needed. */
class MCGLPointParticleRenderer
{
public:

    //! Constructor.
    explicit MCGLPointParticleRenderer(int maxBatchSize = 1024);

    //! Destructor.
    virtual ~MCGLPointParticleRenderer();

    //! Set the shader program to be used.
    virtual void setShaderProgram(MCGLShaderProgram * program);

    //! Set texture to be used.
    virtual void setTexture(GLuint handle);

    //! Set default size of the points.
    virtual void setPointSize(int pointSize);

    //! Enable/disable blending.
    virtual void setAlphaBlend(
        bool useAlphaBlend, GLenum src = GL_SRC_ALPHA, GLenum dst = GL_ONE_MINUS_SRC_ALPHA);

private:

    DISABLE_COPY(MCGLPointParticleRenderer);
    DISABLE_ASSI(MCGLPointParticleRenderer);

    /*! Populate the current batch.
     *  \param particles The vector of particle data to be rendered.
     *  \param camera The camera window. */
    typedef std::vector<MCObject *> ParticleVector;
    virtual void setBatch(const ParticleVector & particles, MCCamera * camera = nullptr);

    //! Render the current particle batch.
    virtual void render();

    GLuint              m_vbo;
    GLuint              m_vba;
    GLuint              m_texture;
    MCGLShaderProgram * m_program;
    int                 m_batchSize;
    int                 m_maxBatchSize;
    MCGLVertex        * m_vertices;
    MCGLColor         * m_colors;
    int                 m_pointSize;
    bool                m_useAlphaBlend;
    GLenum              m_src;
    GLenum              m_dst;

    friend class MCWorldRenderer;
};

#endif // MCGLPOINTPARTICLERENDERER_HH
