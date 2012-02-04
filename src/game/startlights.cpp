// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "startlights.hpp"

Startlights::Startlights()
  : m_state(LightsOff)
  , m_counter(0)
{
}

bool Startlights::updateCounter(MCUint limit)
{
    if (++m_counter > limit)
    {
        m_counter = 0;
        return true;
    }
    return false;
}

void Startlights::run()
{
    const MCUint second = 100;
    switch (m_state)
    {
    case LightsOff:
        if (updateCounter(second))
        {
            m_state = LightsFirstRow;
        }
        break;
    case LightsFirstRow:
        if (updateCounter(second))
        {
            m_state = LightsSecondRow;
        }
        break;
    case LightsSecondRow:
        if (updateCounter(second))
        {
            m_state = LightsThirdRow;
        }
        break;
    case LightsThirdRow:
        if (updateCounter(second))
        {
            m_state = LightsGo;
        }
        break;
    case LightsGo:
        if (updateCounter(second))
        {
            m_state = LightsEnd;
        }
        break;
    case LightsEnd:
        break;
    }
}

Startlights::LightState Startlights::state() const
{
    return m_state;
}
