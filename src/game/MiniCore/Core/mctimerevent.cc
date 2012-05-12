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

#include "mctimerevent.hh"

MCUint MCTimerEvent::m_typeID = MCEvent::registerType();

class MCTimerEventImpl
{
    MCTimerEventImpl(MCUint frequency);
    MCUint frequency;
    friend class MCTimerEvent;
};

MCTimerEventImpl::MCTimerEventImpl(MCUint frequency)
: frequency(frequency)
{
}

MCTimerEvent::MCTimerEvent(MCUint frequency)
: m_pImpl(new MCTimerEventImpl(frequency))
{
}

MCUint MCTimerEvent::typeID()
{
    return MCTimerEvent::m_typeID;
}

MCUint MCTimerEvent::instanceTypeID() const
{
    return MCTimerEvent::m_typeID;
}

MCUint MCTimerEvent::frequency() const
{
    return m_pImpl->frequency;
}

MCTimerEvent::~MCTimerEvent()
{
    delete m_pImpl;
}
