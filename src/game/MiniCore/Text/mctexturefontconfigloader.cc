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

#include "mctexturefontconfigloader.hh"
#include "mctexturefontdata.hh"

#include <cassert>

MCTextureFontConfigLoader::MCTextureFontConfigLoader()
{
}

void MCTextureFontConfigLoader::setConfigPath(QString filePath)
{
    m_filePath = filePath;
}

bool MCTextureFontConfigLoader::loadFonts()
{
    // TODO
    return true;
}

unsigned int MCTextureFontConfigLoader::fonts() const
{
    return m_fonts.size();
}

MCTextureFontData & MCTextureFontConfigLoader::font(unsigned int index) const
{
    assert(index < static_cast<unsigned int>(m_fonts.size()));
    assert(m_fonts.at(index));
    return *m_fonts.at(index);
}

MCTextureFontConfigLoader::~MCTextureFontConfigLoader()
{
    for (MCTextureFontData * font : m_fonts)
    {
        delete font;
    }

    m_fonts.clear();
}
