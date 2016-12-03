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

#ifndef MCFORCEGENERATOR_HH
#define MCFORCEGENERATOR_HH

#include "mcmacros.hh"
#include "mctypes.hh"

#include <memory>

class MCObject;
class MCForceGeneratorImpl;

//! Abstract base class for different force generators
class MCForceGenerator
{
public:

    //! Constructor
    MCForceGenerator();

    //! Destructor
    virtual ~MCForceGenerator();

    //! Update force to the given object
    virtual void updateForce(MCObject & object) = 0;

    //! Enable / disable the force. Enabled by default.
    void enable(bool status);

    //! Return true if enabled.
    bool enabled() const;

private:

    DISABLE_COPY(MCForceGenerator);
    DISABLE_ASSI(MCForceGenerator);

    MCForceGeneratorImpl * const m_pImpl;
};

typedef std::shared_ptr<MCForceGenerator> MCForceGeneratorPtr;

#endif // MCFORCEGENERATOR_HH
