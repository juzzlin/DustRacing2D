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

#ifndef MCEVENT_HH
#define MCEVENT_HH

#include "mctypes.hh"
#include "mcmacros.hh"

class MCObject;

//! Base class for MiniCore events
class MCEvent
{
public:
  //! Constructor
  MCEvent();

  //! Destructor
  virtual ~MCEvent();

  /*! Register a new event type
   * \return The new unique type ID
   */
  static MCUint registerType();

  //! Return class-wide type id
  virtual MCUint instanceTypeID() const = 0;

  //! Accept the event.
  void accept();

  //! Ignore the event. This is the default state.
  void ignore();

  //! Return true, if event was accepted.
  bool accepted() const;

private:

  DISABLE_COPY(MCEvent);
  DISABLE_ASSI(MCEvent);
  static MCUint m_typeCount;
  bool m_accepted;
};

#endif // MCEVENT_HH
