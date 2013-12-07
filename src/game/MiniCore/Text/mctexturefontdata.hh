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

#ifndef MCTEXTUREFONTDATA_HH
#define MCTEXTUREFONTDATA_HH

#include <string>
#include <vector>
#include <map>

//! Data structure used by MCTextureFontConfigLoader.
struct MCTextureFontData
{
    //! Glyph data in the XML config.
    struct Glyph
    {
        //! UPPER-LEFT coordinate. Y-axis points upwards.
        int x0, y0;

        //! LOWER-RIGHT coordinate. Y-axis points upwards.
        int x1, y1;

        //! Name of the glyph e.g. 'a'.
        wchar_t name;
    };

    //! Name of the font.
    std::string name;

    //! Handle to an MCSurface that includes the glyphs.
    std::string surface;

    //! Glyphs.
    std::vector<Glyph> glyphs;

    //! Fallback map.
    std::map<wchar_t, wchar_t> fallback;
};

#endif // MCTEXTUREFONTDATA_HH
