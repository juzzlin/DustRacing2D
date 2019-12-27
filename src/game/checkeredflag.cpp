// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "checkeredflag.hpp"
#include "renderer.hpp"

#include <MCAssetManager>
#include <MCSurface>
#include <MCVector3d>

static const int FLAG_W = 32;
static const int FLAG_H = 24;
static const int V_SPACING = 20;

CheckeredFlag::CheckeredFlag()
  : m_surface(MCAssetManager::surfaceManager().surface("checkeredFlag"))
{
}

void CheckeredFlag::render()
{
    MCVector3dF pos(width() / 2, height() - FLAG_H / 2 - V_SPACING);
    m_surface->setSize(FLAG_W, FLAG_H);
    m_surface->render(nullptr, pos, 0);
}
