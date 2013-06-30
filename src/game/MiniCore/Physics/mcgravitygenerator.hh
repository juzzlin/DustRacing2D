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

#ifndef MCGRAVITYGENERATOR_HH
#define MCGRAVITYGENERATOR_HH

#include "mcforcegenerator.hh"
#include "mcvector3d.hh"
#include "mcmacros.hh"

//! Force generator for gravity
class MCGravityGenerator : public MCForceGenerator
{
public:

    /*! Constructor
     * \param g Gravity vector (G = m * g)
     */
    MCGravityGenerator(const MCVector3d<MCFloat> & g);

    //! Destructor
    virtual ~MCGravityGenerator() {};

    //! \reimp
    virtual void updateForce(MCObject & object);

private:

    DISABLE_COPY(MCGravityGenerator);
    DISABLE_ASSI(MCGravityGenerator);
    MCVector3d<MCFloat> m_g;
};

#endif // MCGRAVITYGENERATOR_HH
