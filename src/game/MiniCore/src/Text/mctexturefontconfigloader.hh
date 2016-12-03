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

#ifndef MCTEXTUREFONTCONFIGLOADER_HH
#define MCTEXTUREFONTCONFIGLOADER_HH

#include <vector>
#include <string>

struct MCTextureFontData;

//! Loads the texture font config XML-file and creates
//! MCTextureFontData's used by MCTextureFontManager.
class MCTextureFontConfigLoader
{
public:

    //! Constructor.
    MCTextureFontConfigLoader();

    //! Destructor.
    ~MCTextureFontConfigLoader();

    //! Set file path.
    void setConfigPath(const std::string & filePath);

    //! Load all fonts found in the current filePath.
    //! \return true if succeeded.
    bool loadFonts();

    //! Get font count.
    unsigned int fonts() const;

    //! Get font data of the given index.
    MCTextureFontData & font(unsigned int index) const;

private:

    typedef std::vector<MCTextureFontData*> FontVector;
    std::string m_filePath;
    FontVector m_fonts;
};

#endif // MCTEXTUREFONTCONFIGLOADER_HH
