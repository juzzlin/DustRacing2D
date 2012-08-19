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

#include <MCSurface>
#include <MCTextureManager>

MainMenu::MainMenu(std::string id, int width, int height)
: Menu(id, width, height, Menu::MS_VERTICAL_LIST)
{
}

void MainMenu::render()
{
    const int w2 = width()  / 2;
    const int h2 = height() / 2;
    MCSurface & back = MCTextureManager::instance().surface("mainMenuBack");
    back.setShaderProgram(&Renderer::instance().masterProgram());
    back.setColor(0.5, 0.5, 0.5, 1.0);
    back.renderScaled(nullptr, MCVector3dF(w2, h2, 0), w2, h2, 0);
    Menu::render();
}
