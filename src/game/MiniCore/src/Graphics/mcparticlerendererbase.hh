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

#ifndef MCPARTICLERENDERERBASE_HH
#define MCPARTICLERENDERERBASE_HH

#include "mcobject.hh"
#include "mcglobjectbase.hh"
#include "mcglvertex.hh"
#include "mcglcolor.hh"
#include "mcgltexcoord.hh"

#include <memory>

class MCParticleRendererBase : public MCGLObjectBase
{
public:

    explicit MCParticleRendererBase(int maxBatchSize = 1024);

    virtual ~MCParticleRendererBase();

    /*! Populate the current batch.
     *  \param particles The vector of particle data to be rendered.
     *  \param camera The camera window. */
    typedef std::vector<MCObject *> ParticleVector;
    virtual void setBatch(ParticleVector & particles, MCCamera * camera = nullptr, bool isShadow = false) = 0;

    //! Render the current particle batch.
    virtual void render() = 0;

    //! Render the current particle batch as shadows.
    virtual void renderShadows() = 0;

    //! Enable/disable blending.
    void setAlphaBlend(bool useAlphaBlend, GLenum src = GL_SRC_ALPHA, GLenum dst = GL_ONE_MINUS_SRC_ALPHA);

    //! \return Set if shadow needs to be rendered
    void setHasShadow(bool hasShadow);

    //! \return True if shadow needs to be rendered
    bool hasShadow() const;

protected:

    //! Set current batch size
    void setBatchSize(int batchSize);

    //! Get current batch size
    int batchSize() const;

    //! Set max batch size
    int maxBatchSize() const;

    bool useAlphaBlend() const;

    GLenum alphaSrc() const;

    GLenum alphaDst() const;

private:

    DISABLE_COPY(MCParticleRendererBase);
    DISABLE_ASSI(MCParticleRendererBase);

    int m_batchSize;

    int m_maxBatchSize;

    bool m_hasShadow;

    bool m_useAlphaBlend;

    GLenum m_src;

    GLenum m_dst;
};

typedef std::shared_ptr<MCParticleRendererBase> MCParticleRendererPtr;

#endif // MCPARTICLERENDERERBASE_HH
