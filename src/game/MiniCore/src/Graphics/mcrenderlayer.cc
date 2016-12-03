// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcrenderlayer.hh"
#include "mccamera.hh"
#include "mcobject.hh"

MCRenderLayer::MCRenderLayer()
    : m_depthTestEnabled(true)
    , m_depthMaskEnabled(true)
{
}

void MCRenderLayer::clear()
{
    m_objectBatches.clear();
    m_particleBatches.clear();
}

void MCRenderLayer::setDepthTestEnabled(bool enable)
{
    m_depthTestEnabled = enable;
}

bool MCRenderLayer::depthTestEnabled() const
{
    return m_depthTestEnabled;
}

void MCRenderLayer::setDepthMaskEnabled(bool enable)
{
    m_depthMaskEnabled = enable;
}

bool MCRenderLayer::depthMaskEnabled() const
{
    return m_depthMaskEnabled;
}

MCRenderLayer::CameraBatchMap & MCRenderLayer::objectBatches()
{
    return m_objectBatches;
}

MCRenderLayer::CameraBatchMap & MCRenderLayer::particleBatches()
{
    return m_particleBatches;
}
