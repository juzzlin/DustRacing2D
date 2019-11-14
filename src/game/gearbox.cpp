// This file is part of Dust Racing 2D.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "gearbox.hpp"

#include <cmath>

Gearbox::Gearbox()
{
}

Gearbox::Gear Gearbox::gear() const
{
    return m_gear;
}

void Gearbox::update(int speedInKmh)
{
    if (m_acceleratorEnabled && m_brakeEnabled)
    {
        m_gear = Gear::Neutral;
    }
    else if (m_brakeEnabled)
    {
        if (speedInKmh == 0)
        {
            if (m_gear != Gear::Stop && m_gear != Gear::Reverse)
            {
                m_gear = Gear::Stop;
                m_stopCounter = 0;
            }
            else
            {
                if (++m_stopCounter > 30) // Wait ~0.5 sec before reverse engages
                {
                    m_gear = Gear::Reverse;
                }
            }
        }
    }
    else if (m_acceleratorEnabled)
    {
        if (m_gear == Gear::Neutral || m_gear == Gear::Stop)
        {
            m_gear = Gear::Forward;
        }
    }
    else
    {
        m_gear = Gear::Neutral;
    }
}

void Gearbox::setAcceleratorEnabled(bool enable)
{
    m_acceleratorEnabled = enable;
}

void Gearbox::setBrakeEnabled(bool enable)
{
    m_brakeEnabled = enable;
}
