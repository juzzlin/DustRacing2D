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

#ifndef MCFRICTIONGENERATOR_HH
#define MCFRICTIONGENERATOR_HH

#include <tr1/memory>

using std::tr1::shared_ptr;

#include "mcforcegenerator.hh"
#include "mcmacros.hh"

class MCFrictionGeneratorImpl;

/*!
 * \class MCFrictionGenerator
 * \brief Force generator for "global" friction between object
 *        and a plane.
 *
 * Force generator for "global" friction. This generator works only
 * in the XY-plane, meaning that only i and j components of
 * velocities are considered. Fast approximation is used to calculate
 * magnitude of the velocity.
 */
class MCFrictionGenerator : public MCForceGenerator
{
public:

  /*! Constructor
   * \param coeff Dynamic friction coefficient.
   */
  MCFrictionGenerator(MCFloat coeff);

  //! Destructor
  virtual ~MCFrictionGenerator();

  //! \reimp
  virtual void updateForce(MCObject * p);

private:
    DISABLE_COPY(MCFrictionGenerator);
    DISABLE_ASSI(MCFrictionGenerator);
    shared_ptr<MCFrictionGeneratorImpl> const m_pImpl;
};

#endif // MCFRICTIONGENERATOR_HH
