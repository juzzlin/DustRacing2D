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

#include "help.hpp"
#include "renderer.hpp"

#include <MCSurface>
#include <MCTextureFont>
#include <MCTextureFontManager>
#include <MCTextureManager>
#include <MCTextureText>

static MCTextureText HELP_TEXT(
    "CONTROLS\n\n"
    "Left  : Turn left\n"
    "Right : Turn right\n"
    "Up    : Accelerate\n"
    "Down  : Brake\n"
    "ESC/Q : Quit\n"
    "P     : Pause"
);

Help::Help(std::string id, int width, int height)
: Menu(id, width, height, Menu::MS_VERTICAL_LIST)
, m_back(MCTextureManager::instance().surface("helpBack"))
, m_monospace(MCTextureFontManager::instance().font("default"))
{
    m_back.setShaderProgram(&Renderer::instance().masterProgram());
    m_back.setColor(0.5, 0.5, 0.5, 1.0);
}

void Help::render()
{
    const int w2 = width()  / 2;
    const int h2 = height() / 2;
    m_back.renderScaled(nullptr, MCVector3dF(w2, h2, 0), w2, h2, 0);

    HELP_TEXT.setGlyphSize(20, 20);
    HELP_TEXT.render(
        width()  / 2 - HELP_TEXT.width()  / 2,
        height() / 2 + HELP_TEXT.height() / 2, nullptr, m_monospace);

    Menu::render();
}
