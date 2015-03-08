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

#ifndef MCSURFACECONFIGLOADER_HH
#define MCSURFACECONFIGLOADER_HH

#include <string>
#include <map>
#include <vector>
#include <memory>

#include <MCGLEW> // For GLenum.

struct MCSurfaceMetaData;

class QDomElement;
class QDomNode;

//! Loads the surface config.
class MCSurfaceConfigLoader
{
public:

    //! Constructor.
    MCSurfaceConfigLoader();

    //! Load all surfaces found in filePath.
    //! \return true if succeeded.
    bool load(const std::string & filePath);

    //! Get surface count.
    unsigned int surfaceCount() const;

    //! Get surface data of given index.
    const MCSurfaceMetaData & surface(unsigned int index) const;

private:

    //! Convert alpha blend function string to enum.
    GLenum alphaBlendStringToEnum(const std::string & function) const;

    typedef std::shared_ptr<MCSurfaceMetaData> SurfaceDataPtr;

    void parseAttributes(const QDomElement & element, SurfaceDataPtr newData, const std::string & baseImagePath);

    void parseChildNodes(const QDomNode & node, SurfaceDataPtr newData);

    std::vector<SurfaceDataPtr> m_surfaces;

    typedef std::map<std::string, GLenum> BlendFuncMap;
    BlendFuncMap m_blendFuncMap;
};

#endif // MCSURFACECONFIGLOADER_HH
