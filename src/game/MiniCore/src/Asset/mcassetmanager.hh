// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCASSETMANAGER_HH
#define MCASSETMANAGER_HH

#include <string>

#include "mcsurfacemanager.hh"
#include "mctexturefontmanager.hh"
#include "mcmeshmanager.hh"

/*! Singleton class that handles loading of assets and instantiates
 *  all sub-managers (MCSurfaceManager, MCTextureFontManager, MCMeshManager) */
class MCAssetManager
{
public:

    //! Constructor.
    explicit MCAssetManager(
        const std::string & baseDataPath = "",
        const std::string & surfaceConfigPath = "",
        const std::string & fontConfigPath = "",
        const std::string & meshConfigPath = "");

    //! \return The singleton instance.
    static MCAssetManager & instance();

    static MCSurfaceManager & surfaceManager();

    static MCTextureFontManager & textureFontManager();

    static MCMeshManager & meshManager();

    //! Loads all assets.
    void load();

    //! Destructor.
    ~MCAssetManager();

private:

    void loadSurfaces();
    void loadFonts();
    void loadMeshes();

    static MCAssetManager * m_instance;
    MCSurfaceManager      * m_surfaceManager;
    MCTextureFontManager  * m_textureFontManager;
    MCMeshManager         * m_meshManager;
    std::string             m_baseDataPath;
    std::string             m_surfaceConfigPath;
    std::string             m_fontConfigPath;
    std::string             m_meshConfigPath;
};

#endif // MCASSETMANAGER_HH
