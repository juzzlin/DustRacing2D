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

#include "speedometer.h"
#include "car.h"

#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTextureManager"

Speedometer::Speedometer()
  : m_pCar(nullptr)
  , m_body(MCTextureManager::instance().surface("speedometer-body"))
  , m_hand(MCTextureManager::instance().surface("speedometer-hand"))
  , m_center(MCTextureManager::instance().surface("speedometer-center"))
  , m_angle(180.0)
  , m_bodyX(16 + m_body.width() / 2)
  , m_bodyY(16 + m_body.height() / 2)
{
}

void Speedometer::setCarToFollow(const Car & car)
{
    m_pCar = &car;
}

void Speedometer::render()
{
    // The length of the velocity vector is typically in the range
    // of 0..20. This formula scales it to degrees. There's no exact
    // calulations behined this scaling, these values just feel quite
    // realistic in practice.
    MCFloat newAngle = 180 - m_pCar->velocity().lengthFast() * 270 / 12;

    // Make the stored angle to follow the real angle.
    if (m_angle < newAngle - 1.0f)
    {
        m_angle += 0.5f;
    }
    else if (m_angle > newAngle + 1.0f)
    {
        m_angle -= 0.5f;
    }

    // Render the components.
    m_body.render(nullptr, m_bodyX, m_bodyY, 0, 0);
    m_hand.render(nullptr, m_bodyX, m_bodyY, 0, m_angle);
    m_center.render(nullptr, m_bodyX, m_bodyY, 0, m_angle);
}
