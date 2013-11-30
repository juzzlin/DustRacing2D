// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "mctextureglyph.hh"

#include <cassert>

MCTextureGlyph::MCTextureGlyph(UV uv0, UV uv1)
{
    m_uv[0] = uv0;
    m_uv[1] = UV(uv1.m_u, uv0.m_v);
    m_uv[2] = uv1;
    m_uv[3] = UV(uv0.m_u, uv1.m_v);
}

const MCTextureGlyph::UV & MCTextureGlyph::uv(MCUint vertex) const
{
    assert(vertex < 4);
    return m_uv[vertex];
}
