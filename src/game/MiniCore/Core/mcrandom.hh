// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2009 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCRANDOM_HH
#define MCRANDOM_HH

#include "mcmacros.hh"
#include "mctypes.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <memory>

class MCRandomImpl;

//! MCRandom number LUT.
class MCRandom
{
public:

    //! Get next random value in the table
    static MCFloat getValue();

    //! Return a random 2d vector
    static MCVector2d<MCFloat> randomVector2d();

    //! Return a random 3d vector
    static MCVector3d<MCFloat> randomVector3d();

private:

    //! Constructor disabled
    MCRandom();

    //! Disable copy constructor
    DISABLE_COPY(MCRandom);

    //! Disable assignment
    DISABLE_ASSI(MCRandom);

    static std::shared_ptr<MCRandomImpl> const m_pImpl;
};

#endif // MCRANDOM_HH
