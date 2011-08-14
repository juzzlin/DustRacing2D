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

#ifndef MCCOLLISIONEVENT_HH
#define MCCOLLISIONEVENT_HH

#include "mcevent.hh"
#include "mcvector3d.hh"

class MCObject;
class MCCollisionEventImpl;

/*! \class MCCollisionEvent
 *  \brief Event sent when two objects collides.
 *
 *  The receiving object must accept the event in their
 *  event handlers (event() or collisionEvent()) in order to
 *  trigger collision physics. This way objects can filter
 *  with which objects they want to collide.
 */
class MCCollisionEvent : public MCEvent
{
public:
  /*! Constructor
   * \param p This event is sent to an object that has collided with p
   */
  explicit MCCollisionEvent(MCObject * p);

  //! Destructor
  ~MCCollisionEvent();

  //! Get colling object
  MCObject * collidingObject() const;

  //! Return the typeID
  static UINT typeID();

  //! \reimp
  virtual UINT instanceTypeID() const;

private:

  DISABLE_COPY(MCCollisionEvent);
  DISABLE_ASSI(MCCollisionEvent);

  static UINT m_typeID;
  MCCollisionEventImpl * const m_pImpl;
};

#endif // MCCOLLISIONEVENT_HH
