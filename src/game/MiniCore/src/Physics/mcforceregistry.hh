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

#ifndef MCFORCEREGISTRY_HH
#define MCFORCEREGISTRY_HH

#include "mcmacros.hh"
#include "mcforcegenerator.hh"

#include <map>
#include <memory>
#include <set>
#include <vector>

class MCObject;

/*! \class MCForceRegistry
 *  \brief MCForceRegistry stores object-force -pairs
 */
class MCForceRegistry
{
public:

    //! Constructor.
    MCForceRegistry();

    /*! Add given force generator to given object
     * \param generator Force generator to be attached.
     * \param object Target object. */
    void addForceGenerator(MCForceGeneratorPtr generator, MCObject & object);

    /*! Remove given force generator to given object
     * \param generator Force generator to be matched.
     * \param object Object to be matched */
    void removeForceGenerator(MCForceGeneratorPtr generator, MCObject & object);

    /*! Remove all force generators bound to given object.
     * \param object Object to be matched */
    void removeForceGenerators(MCObject & object);

    //! Update force generators
    void update();

    //! Clear registry
    void clear();

private:

    DISABLE_COPY(MCForceRegistry);
    DISABLE_ASSI(MCForceRegistry);

    // Prefer map and set here for iteration performance.
    typedef std::vector<MCForceGeneratorPtr> Registry;
    typedef std::map<MCObject *, Registry> RegistryHash;
    RegistryHash m_registryHash;
};

#endif // MCFORCEREGISTRY_HH
