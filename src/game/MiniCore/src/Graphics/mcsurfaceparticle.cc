// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcsurfaceparticle.hh"

MCSurfaceParticle::MCSurfaceParticle(const std::string & typeID, MCSurface & surface)
: MCParticle(typeID)
, m_color(1.0, 1.0, 1.0, 1.0)
, m_surface(surface)
, m_hasShadow(false)
, m_useAlphaBlend(false)
, m_src(0)
, m_dst(0)
{
}

void MCSurfaceParticle::setColor(const MCGLColor & color)
{
    m_color = color;
}

const MCGLColor & MCSurfaceParticle::color() const
{
    return m_color;
}

MCSurface & MCSurfaceParticle::surface()
{
    return m_surface;
}

void MCSurfaceParticle::setHasShadow(bool hasShadow)
{
    m_hasShadow = hasShadow;
}

bool MCSurfaceParticle::hasShadow() const
{
    return m_hasShadow;
}

void MCSurfaceParticle::setAlphaBlend(bool useAlphaBlend, GLenum src, GLenum dst)
{
    m_useAlphaBlend = useAlphaBlend;
    m_src           = src;
    m_dst           = dst;
}

bool MCSurfaceParticle::useAlphaBlend() const
{
    return m_useAlphaBlend;
}

GLenum MCSurfaceParticle::alphaSrc() const
{
    return m_src;
}

GLenum MCSurfaceParticle::alphaDst() const
{
    return m_dst;
}

