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

#include "credits.hpp"
#include "renderer.hpp"

#include <MCSurface>
#include <MCTextureFont>
#include <MCTextureFontManager>
#include <MCTextureManager>
#include <MCTextureText>

static MCTextureText CREDITS_TEXT(
    "PROGRAMMING\n\n"
    "* Jussi Lind\n\n"
    "GRAPHICS\n\n"
    "* Jussi Lind\n"
    "* Ville Makiranta\n\n"
    "RACE TRACKS\n\n"
    "* Jussi Lind\n"
    "* Antti Kervinen\n\n"
    "FONT\n\n"
    "* Ubuntu Monospace\n\n"
    "SPECIAL THANKS\n\n"
    "Ubuntu, Gimp\nInkScape, CMake\nGCC, Qt, Git"
);

Credits::Credits(std::string id, int width, int height)
: Menu(id, width, height, Menu::MS_VERTICAL_LIST)
{
}

void Credits::render()
{
    const int w2 = width()  / 2;
    const int h2 = height() / 2;
    MCSurface & back = MCTextureManager::instance().surface("creditsBack");
    back.setShaderProgram(&Renderer::instance().masterProgram());
    back.renderScaled(nullptr, MCVector3dF(w2, h2, 0), w2, h2, 0);
    back.setColor(0.5, 0.5, 0.5, 1.0);

    MCTextureFont & monospace = MCTextureFontManager::instance().font("default");
    CREDITS_TEXT.setGlyphSize(20, 20);
    CREDITS_TEXT.render(
        width()  / 2 - CREDITS_TEXT.width()  / 2,
        height() / 2 + CREDITS_TEXT.height() / 2, nullptr, monospace);

    Menu::render();
}
