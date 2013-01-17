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

#include "mcassetmanager.hh"
#include "mclogger.hh"

#include <cassert>

MCAssetManager * MCAssetManager::m_instance = nullptr;

MCAssetManager::MCAssetManager(
    const std::string & baseDataPath,
    const std::string & surfaceConfigPath,
    const std::string & fontConfigPath,
    const std::string & meshConfigPath)
: m_surfaceManager(new MCSurfaceManager)
, m_textureFontManager(new MCTextureFontManager(*m_surfaceManager))
, m_meshManager(new MCMeshManager)
, m_baseDataPath(baseDataPath)
, m_surfaceConfigPath(surfaceConfigPath)
, m_fontConfigPath(fontConfigPath)
, m_meshConfigPath(meshConfigPath)
{
    assert(!MCAssetManager::m_instance);
    MCAssetManager::m_instance = this;
}

MCAssetManager & MCAssetManager::instance()
{
    assert(MCAssetManager::m_instance);
    return *MCAssetManager::m_instance;
}

MCSurfaceManager & MCAssetManager::surfaceManager()
{
    assert(instance().m_surfaceManager);
    return *instance().m_surfaceManager;
}

MCTextureFontManager & MCAssetManager::textureFontManager()
{
    assert(instance().m_textureFontManager);
    return *instance().m_textureFontManager;
}

MCMeshManager & MCAssetManager::meshManager()
{
    assert(instance().m_meshManager);
    return *instance().m_meshManager;
}

void MCAssetManager::load()
{
    loadSurfaces();
    loadFonts();
    loadMeshes();
}

void MCAssetManager::loadSurfaces()
{
    if (m_surfaceConfigPath != "")
    {
        MCLogger().info() << "Loading surface config from '" << m_surfaceConfigPath << "'..";
        m_surfaceManager->load(m_surfaceConfigPath, m_baseDataPath);
    }
}

void MCAssetManager::loadFonts()
{
    if (m_fontConfigPath != "")
    {
        MCLogger().info() << "Loading font config from '" << m_fontConfigPath << "'..";
        m_textureFontManager->load(m_fontConfigPath);
    }
}

void MCAssetManager::loadMeshes()
{
    if (m_meshConfigPath != "")
    {
        MCLogger().info() << "Loading mesh config from '" << m_meshConfigPath << "'..";
        m_meshManager->load(m_meshConfigPath, m_baseDataPath);
    }
}

MCAssetManager::~MCAssetManager()
{
    delete m_surfaceManager;
    delete m_textureFontManager;
    delete m_meshManager;
}
