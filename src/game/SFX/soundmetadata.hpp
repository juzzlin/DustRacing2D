// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCSOUNDMETADATA_HPP
#define MCSOUNDMETADATA_HPP

#include "mctypes.hh"

#include <string>
#include <vector>

namespace SFX {

//! Sound data structure used when reading sound config
struct SoundMetaData
{
    //! Constructor.
    SoundMetaData() : isMusic(false) {}

    //! True if data is to be treated like a music file.
    bool isMusic;

    //! Handle of the sound.
    std::string handle;

    //! File path.
    std::string filePath;
};

} // namespace SFX

#endif // SOUNDMETADATA_HPP
