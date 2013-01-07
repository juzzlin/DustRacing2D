// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCIMPULSEGENERATOR_HH
#define MCIMPULSEGENERATOR_HH

#include "mctypes.hh"
#include "mcvector3d.hh"
#include <vector>

class MCObject;
class MCContact;

//! Generates impulses due to detected collisions.
class MCImpulseGenerator
{
public:

    //! Constructor.
    MCImpulseGenerator();

    //! Destructor.
    ~MCImpulseGenerator() {};

    //! Generate impulses to the given objects according to current contacts.
    //! Delete contacts.
    void generateImpulsesFromDeepestContacts(std::vector<MCObject *> & objs);

    //! Resolve positions of the given objects according to current contacts.
    //! Delete contacts.
    void resolvePositions(std::vector<MCObject *> & objs, MCFloat accuracy);

private:

    void generateImpulsesFromContact(
        MCObject & pa, MCObject & pb, const MCContact & contact,
        const MCVector3dF & linearImpulse,
        MCFloat restitution);

    void displace(MCObject & pa, MCObject & pb, const MCVector3dF & displacement);

    MCContact * getDeepestInterpenetration(const std::vector<MCContact *> & contacts);
};

#endif // MCIMPULSEGENERATOR_HH
