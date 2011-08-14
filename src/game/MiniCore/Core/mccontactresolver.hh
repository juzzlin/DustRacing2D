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

#ifndef MCCONTACTRESOLVER_HH
#define MCCONTACTRESOLVER_HH

#include "mcmacros.hh"

#include <tr1/memory>

using std::tr1::shared_ptr;

class MCContactResolverImpl;
class MCObject;

//! Contact resolver / collision detector
class MCContactResolver
{
public:
  //! Constructor
  MCContactResolver();

  //! Destructor
  virtual ~MCContactResolver();

  /*! Process a suspected collision
   * \param p1 Pointer to the 1st object
   * \param p2 Pointer to the 2nd object
   * \return true if objects really collided
   */
  bool processPossibleCollision(MCObject * p1, MCObject * p2);

private:

  DISABLE_COPY(MCContactResolver);
  DISABLE_ASSI(MCContactResolver);
  shared_ptr<MCContactResolverImpl> const m_pImpl;
  friend class MCContactResolverImpl;
};

#endif // MCCONTACTRESOLVER_HH
