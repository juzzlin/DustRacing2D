// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "mainmenu.hpp"

#include "renderer.hpp"
#include "scene.hpp"
#include "statemachine.hpp"
#include "menuitem.hpp"
#include "menumanager.hpp"

#include <MCLogger>
#include <MCSurface>
#include <MCTextureManager>
#include <MCTextureFont>
#include <MCTextureFontManager>
#include <MCTextureText>

#include <cassert>
#include <sstream>

MainMenu::MainMenu(std::string id, int width, int height)
: Menu(id, width, height, Menu::MS_VERTICAL_LIST)
{
}

void MainMenu::render()
{
    MCSurface & back = MCTextureManager::instance().surface("menu");
    back.setShaderProgram(&Renderer::instance().masterProgram());
    back.renderScaled(nullptr, MCVector3dF(0, 0, 0), width(), height(), 0);
    Menu::render();
}
