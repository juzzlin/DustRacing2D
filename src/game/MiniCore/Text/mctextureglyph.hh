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

#ifndef MCTEXTUREGLYPH_HH
#define MCTEXTUREGLYPH_HH

#include "mctypes.hh"

#include <MCGLEW>

//! Textured glyph used by MCTextureFont.
class MCTextureGlyph
{
public:

    //! Texture coordinates of a vertex.
    struct UV
    {
        UV() : m_u(0), m_v(0) {}
        UV(GLfloat u, GLfloat v) : m_u(u), m_v(v) {}
        GLfloat m_u, m_v;
    };

    /*! Constructor.
     * \param uv0 Upper left texture coordinates.
     * \param uv0 Lower right texture coordinates. */
    MCTextureGlyph(UV uv0, UV uv1);

    /*! Return UV for the given vertex index:
     *
     *  0 - 1
     *  |   |
     *  3 - 2
     */
    const UV & uv(MCUint vertex) const;

private:

    UV  m_uv[4];
};

#endif // MCTEXTUREGLYPH_HH
