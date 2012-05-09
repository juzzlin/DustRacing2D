// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

class MCSurface;
class MCSurfaceParticleImpl;

/*! \class MCSurfaceParticle
 *  \brief A particle that renders as a MCSurface. Can be inherited.
 */
class MCSurfaceParticle : public MCParticle
{
public:

    //! (Re-)init the particle.
    virtual void init(const MCVector3d<MCFloat> & location, MCFloat radius, MCUint lifeTime,
        MCSurface * pSurface);

    //! Create a new particle or return recycled one
    static MCSurfaceParticle * create();

    //! \reimp
    virtual void recycle();

protected:

    //! Constructor
    MCSurfaceParticle();

    //! Destructor
    virtual ~MCSurfaceParticle();

private:

    //! \reimp
    virtual void init(const MCVector3d<MCFloat> & location, MCFloat radius, MCUint lifeTime);

    //! Recycler object
    static MCRecycler<MCSurfaceParticle> m_recycler;
    friend class MCRecycler<MCSurfaceParticle>;

    DISABLE_COPY(MCSurfaceParticle);
    DISABLE_ASSI(MCSurfaceParticle);
    MCSurfaceParticleImpl * const m_pImpl;
};

#endif // MCSURFACEPARTICLE_HH
