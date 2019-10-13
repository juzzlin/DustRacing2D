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

unsigned int MCTimerEvent::m_typeId = MCEvent::registerType();

class MCTimerEventImpl
{
    MCTimerEventImpl(unsigned int frequency);
    unsigned int frequency;
    friend class MCTimerEvent;
};

MCTimerEventImpl::MCTimerEventImpl(unsigned int frequency)
  : frequency(frequency)
{
}

MCTimerEvent::MCTimerEvent(unsigned int frequency)
  : m_pImpl(new MCTimerEventImpl(frequency))
{
}

unsigned int MCTimerEvent::typeId()
{
    return MCTimerEvent::m_typeId;
}

unsigned int MCTimerEvent::instanceTypeId() const
{
    return MCTimerEvent::m_typeId;
}

unsigned int MCTimerEvent::frequency() const
{
    return m_pImpl->frequency;
}

MCTimerEvent::~MCTimerEvent()
{
    delete m_pImpl;
}
