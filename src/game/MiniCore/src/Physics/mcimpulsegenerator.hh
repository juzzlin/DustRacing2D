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

#include "mcvector2d.hh"

#include <vector>

class MCObject;
class MCContact;

/*! Resolves collisions with a sequential impulse solver: the impulses of all contact points
 *  are iterated together and accumulated, so that e.g. a box lying flat against a wall is
 *  supported by both of its corners. Impulses take the rotational inertia of the bodies into
 *  account. Overlaps are corrected separately by moving the bodies apart. */
class MCImpulseGenerator
{
public:
    MCImpulseGenerator();

    /*! Generate impulses to the given objects according to their current contacts and delete the contacts.
     *  \param timeStep Time step in seconds. */
    void generateImpulses(std::vector<MCObject *> & objects, float timeStep);

    //! Resolve positions of the given objects according to their current contacts and delete the contacts.
    void resolvePositions(std::vector<MCObject *> & objects, float accuracy);

private:
    void displace(MCObject & object, MCObject & other, const MCVector2dF & displacement);

    MCContact * getDeepestInterpenetration(const std::vector<MCContact *> & contacts);
};

#endif // MCIMPULSEGENERATOR_HH
