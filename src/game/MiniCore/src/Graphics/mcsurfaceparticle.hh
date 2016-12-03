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

#ifndef MCSURFACEPARTICLE_HH
#define MCSURFACEPARTICLE_HH

#include "mcparticle.hh"
#include "mccamera.hh"
#include "mcglcolor.hh"
#include "mcglshaderprogram.hh"
#include "mcsurface.hh"

#include <string>

class MCSurface;

/*! \class MCSurfaceParticle
 *  \brief A particle that renders as an MCSurface. Can be inherited.
 *
 *  \see MCSurfaceParticleRenderer.
 */
class MCSurfaceParticle : public MCParticle
{
public:

    /*! Constructor.
     *  \param viewId id for the particle. All particles of a same kind should
     *  use the same id. E.g. batching is carried out based on this id.
     *  \param surface Surface used by the particle. */
    MCSurfaceParticle(const std::string & viewId, MCSurface & surface);

    //! Destructor.
    virtual ~MCSurfaceParticle() {};

    //! Set color
    void setColor(const MCGLColor & color);

    //! Get the color.
    const MCGLColor & color() const;

    MCSurface & surface();

    //! Enable/disable blending.
    void setAlphaBlend(bool useAlphaBlend, GLenum src = GL_SRC_ALPHA, GLenum dst = GL_ONE_MINUS_SRC_ALPHA);

    bool useAlphaBlend() const;

    GLenum alphaSrc() const;

    GLenum alphaDst() const;

    //! \return Set if shadow needs to be rendered
    void setHasShadow(bool hasShadow);

    //! \return True if shadow needs to be rendered
    bool hasShadow() const;

private:

    DISABLE_COPY(MCSurfaceParticle);
    DISABLE_ASSI(MCSurfaceParticle);

    MCGLColor m_color;

    MCSurface & m_surface;

    bool m_hasShadow;

    bool m_useAlphaBlend;

    GLenum m_src;

    GLenum m_dst;
};

#endif // MCSURFACEPARTICLE_HH
