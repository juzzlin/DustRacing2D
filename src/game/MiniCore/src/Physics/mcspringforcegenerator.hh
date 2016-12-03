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

#ifndef MCSPRINGFORCEGENERATOR_HH
#define MCSPRINGFORCEGENERATOR_HH

#include "mcforcegenerator.hh"
#include "mcmacros.hh"

#include <memory>

/*! \class MCSpringForceGenerator
 *  \brief Force generator for (constrained) springs.
 *
 * By carefully selecting the nominal length, min length, max length and the spring
 * coefficient one can create traditional springs as well as rods and cables.
 */
class MCSpringForceGenerator : public MCForceGenerator
{
public:

    /*! Constructor
     * \param object2 The another end of the spring.
     * \param coeff   Spring coefficient (F = -coeff * x)
     * \param length  Nominal length of the spring.
     * \param min     Min length of the spring.
     * \param max     Max length of the spring.
     */
    MCSpringForceGenerator(
        MCObject & object2, MCFloat coeff, MCFloat length, MCFloat min, MCFloat max);

    //! Destructor
    virtual ~MCSpringForceGenerator();

    /*! \brief Update the force with respect to object1.
     * NOTE!: You must create generators for the both ends of the spring.
     */
    virtual void updateForce(MCObject & object1);

private:

    DISABLE_COPY(MCSpringForceGenerator);
    DISABLE_ASSI(MCSpringForceGenerator);
    MCObject * m_p2;
    MCFloat m_coeff;
    MCFloat m_length;
    MCFloat m_min;
    MCFloat m_max;
};

typedef std::shared_ptr<MCSpringForceGenerator> MCSpringForceGeneratorPtr;

#endif // MCSPRINGFORCEGENERATOR_HH
