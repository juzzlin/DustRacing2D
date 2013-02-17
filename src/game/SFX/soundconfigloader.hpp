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

#ifndef SOUNDCONFIGLOADER_HPP
#define SOUNDCONFIGLOADER_HPP

#include "soundmetadata.hpp"

#include <vector>
#include <memory>

namespace SFX {

//! Loads the sound config.
class SoundConfigLoader
{
public:

    //! Constructor.
    SoundConfigLoader();

    /*! Load all sounds found in configFilePath.
     *  \param configFilePath Path to the sound config file.
     *  \return true if succeeded. */
    bool load(const std::string & configFilePath);

    //! Get sound count.
    unsigned int soundCount() const;

    //! Get sound data of given index.
    const SoundMetaData & sound(unsigned int index) const;

private:

    typedef std::shared_ptr<SoundMetaData> SoundMetaDataPtr;
    std::vector<SoundMetaDataPtr> m_sounds;
};

} // namespace SFX

#endif // SOUNDCONFIGLOADER_HPP
