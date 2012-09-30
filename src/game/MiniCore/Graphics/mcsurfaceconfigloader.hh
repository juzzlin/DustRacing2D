// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCSURFACECONFIGLOADER_HH
#define MCSURFACECONFIGLOADER_HH

#include <string>
#include <vector>

#include <GL/gl.h> // For GLenum.

class MCSurfaceData;

//! Manages loading of the surface config.
class MCSurfaceConfigLoader
{
public:

    //! Constructor.
    MCSurfaceConfigLoader();

    //! Destructor.
    ~MCSurfaceConfigLoader();

    //! Set file path.
    void setConfigPath(const std::string & filePath);

    //! Load all surfaces found in the current filePath.
    //! \return true if succeeded.
    bool load();

    //! Get surface count.
    unsigned int surfaceCount() const;

    //! Get surface data of given index.
    MCSurfaceData & surface(unsigned int index) const;

private:

    //! Convert alpha blend function string to enum.
    GLenum alphaBlendStringToEnum(const std::string & function) const;

    std::string m_filePath;
    std::vector<MCSurfaceData *> m_surfaces;
};

#endif // MCSURFACECONFIGLOADER_HH
