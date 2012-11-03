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

#include "checkeredflag.hpp"
#include "renderer.hpp"

#include <MCSurface>
#include <MCSurfaceManager>
#include <MCVector3d>

static const int   FLAG_W           = 32;
static const int   FLAG_H           = 24;
static const int   V_SPACING        = 20;
static const float APPEARANCE_SPEED = 0.05f;

CheckeredFlag::CheckeredFlag()
: m_surface(MCSurfaceManager::instance().surface("checkeredFlag"))
{
    m_surface.setShaderProgram(&Renderer::instance().program("master"));
}

void CheckeredFlag::setDimensions(MCUint width, MCUint height)
{
    OverlayBase::setDimensions(width, height);
}

void CheckeredFlag::render()
{
    MCVector3dF pos(width() / 2, height() - FLAG_H / 2 - V_SPACING);
    m_surface.renderScaled(nullptr, pos, FLAG_W / 2, FLAG_H / 2, 0);
}
