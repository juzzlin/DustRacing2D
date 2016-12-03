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

#include "mcevent.hh"
#include "mcobject.hh"

MCUint MCEvent::m_typeCount = 0;

MCEvent::MCEvent()
: m_accepted(false)
{}

MCUint MCEvent::registerType()
{
    return ++MCEvent::m_typeCount;
}

void MCEvent::accept()
{
    m_accepted = true;
}

void MCEvent::ignore()
{
    m_accepted = false;
}

bool MCEvent::accepted() const
{
    return m_accepted;
}

MCEvent::~MCEvent()
{}
