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

#ifndef MCSPRINGFORCEGENERATOR2DFAST_HH
#define MCSPRINGFORCEGENERATOR2DFAST_HH

#include "mcforcegenerator.hh"
#include "mcmacros.hh"

#include <memory>

/*! \class MCSpringForceGenerator2dFast
 *  \brief Fast force generator for (constrained) springs in 2d.
 *
 * By carefully selecting the nominal length, min length, max length and the spring
 * coefficient one can create traditional springs as well as rods and cables.
 *
 * This generator works only in the XY-plane, meaning that only i and j components of
 * coordinates are considered. Fast approximation is used to calculate
 * the distance between end nodes.
 */
class MCSpringForceGenerator2dFast : public MCForceGenerator
{
public:

  /*! Constructor
   * \param object2 The another end of the spring.
   * \param coeff   Spring coefficient (F = -coeff * x)
   * \param length  Nominal length of the spring.
   * \param min     Min length of the spring.
   * \param max     Max length of the spring.
   */
  MCSpringForceGenerator2dFast(MCObject & object2,
      MCFloat coeff, MCFloat length, MCFloat min, MCFloat max);

  //! Destructor
  virtual ~MCSpringForceGenerator2dFast();

  /*! \brief Update the force with respect to object1.
   * Only x and y coordinates are considered.
   * NOTE!: You must create generators for the both ends of the spring.
   */
  virtual void updateForce(MCObject & object1);

private:

    DISABLE_COPY(MCSpringForceGenerator2dFast);
    DISABLE_ASSI(MCSpringForceGenerator2dFast);
    MCObject * m_p2;
    MCFloat m_coeff;
    MCFloat m_length;
    MCFloat m_min;
    MCFloat m_max;
};

typedef std::shared_ptr<MCSpringForceGenerator2dFast> MCSpringForceGenerator2dFastPtr;

#endif // MCSPRINGFORCEGENERATOR2DFAST_HH
