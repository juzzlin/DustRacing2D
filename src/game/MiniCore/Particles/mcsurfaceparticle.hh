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
#include "mccamera.hh"
#include "mcsurface.hh"

#include <string>

class MCSurface;

/*! \class MCSurfaceParticle
 *  \brief A particle that renders as an MCSurface. Can be inherited.
 *
 *  Use MCObject::setSurface() to set the surface.
 */
class MCSurfaceParticle : public MCParticle
{
public:

    //! Constructor
    MCSurfaceParticle(const std::string & viewId);

    //! \reimp
    virtual void renderShadow(MCCamera *)
    {
        // Do nothing
    }

    //! Destructor
    virtual ~MCSurfaceParticle();

private:

    DISABLE_COPY(MCSurfaceParticle);
    DISABLE_ASSI(MCSurfaceParticle);
};

#endif // MCSURFACEPARTICLE_HH
