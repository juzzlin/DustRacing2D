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

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTextureManager"
#include "MiniCore/Text/MCTextureFontManager"
#include "MiniCore/Text/MCTextureText"

#include <sstream>

Speedometer::Speedometer()
  : m_pCar(nullptr)
  , m_surface(MCTextureManager::instance().surface("speedometer"))
{
}

void Speedometer::setCarToFollow(const Car & car)
{
    m_pCar = &car;
}

void Speedometer::render()
{
    m_surface.render(nullptr,
         16 + m_surface.width() / 2,
         16 + m_surface.height() / 2,
         0,
         0);
}

Speedometer::~Speedometer()
{
}
