// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "carstatusview.hpp"
#include "car.hpp"
#include "renderer.hpp"

#include <MCAssetManager>
#include <MCSurface>

CarStatusView::CarStatusView()
  : m_body(MCAssetManager::instance().surfaceManager().surface("tireStatusIndicatorBody"))
  , m_tires(MCAssetManager::instance().surfaceManager().surface("tireStatusIndicatorTires"))
  , m_car(nullptr)
{
    setDimensions(m_body->height(), m_tires->width());

    m_body->setColor(MCGLColor(1.0f, 1.0f, 1.0f, 0.9f));
}

void CarStatusView::render()
{
    const float damageLevel = m_car->damageLevel();
    m_body->setColor(MCGLColor(1.0f, damageLevel, damageLevel, 0.9f));
    m_body->render(nullptr, MCVector3dF(x(), y(), 0), 0);

    const float tireWearLevel = m_car->tireWearLevel();
    m_tires->setColor(MCGLColor(1.0f, tireWearLevel, tireWearLevel, 0.9f));
    m_tires->render(nullptr, MCVector3dF(x(), y(), 0), 0);
}

void CarStatusView::setCarToFollow(const Car & car)
{
    m_car = &car;
}
