// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include "crashoverlay.hpp"
#include "car.hpp"

#include <MCAssetManager>
#include <MCGLColor>
#include <MCSurface>

CrashOverlay::CrashOverlay()
  : m_surface(MCAssetManager::instance().surfaceManager().surface("crashOverlay"))
  , m_alpha(1.0f)
  , m_car(nullptr)
  , m_isTriggered(false)
{
    m_surface->material()->setAlphaBlend(true);
}

void CrashOverlay::setDimensions(size_t width, size_t height)
{
    OverlayBase::setDimensions(width, height);
}

void CrashOverlay::setCarToFollow(Car & car)
{
    m_car = &car;
}

void CrashOverlay::render()
{
    if (m_isTriggered)
    {
        glDisable(GL_DEPTH_TEST);

        const auto halfWidth = width() / 2;
        const auto halfHeight = height() / 2;

        m_surface->setColor({ 1.0, 1.0, 1.0, m_alpha });
        m_surface->setSize(width(), height());
        m_surface->render(nullptr, MCVector3dF(halfWidth, halfHeight, 0), 0);
    }
}

bool CrashOverlay::update()
{
    if (m_car && m_car->hadHardCrash())
    {
        m_isTriggered = true;
        m_alpha = 0.75f;
    }
    else if (m_isTriggered)
    {
        m_alpha *= 0.97f;
        if (m_alpha < 0.01f)
        {
            m_isTriggered = false;
        }
    }

    return false;
}
