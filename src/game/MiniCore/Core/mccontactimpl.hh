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

#ifndef MCCONTACTIMPL_HH
#define MCCONTACTIMPL_HH

#include "mcrecycler.hh"

//! Implementation class for MCContact.
class MCContactImpl
{
  MCContactImpl();
  MCObject * m_pObject;
  MCVector2d<MCFloat> m_contactPoint;
  MCVector2d<MCFloat> m_contactNormal;
  MCFloat m_interpenetrationDepth;
  static MCRecycler<MCContact> m_recycler;
  friend class MCContact;
};

#endif // MCCONTACTIMPL_HH
