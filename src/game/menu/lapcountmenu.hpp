// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef LAPCOUNTMENU_HPP
#define LAPCOUNTMENU_HPP

#include "surfacemenu.hpp"

#include <MCTextureFont>

class LapCountMenu : public SurfaceMenu
{
public:
    static std::string MenuId;

    LapCountMenu(int width, int height);

    virtual void render() override;

private:
    MCTextureFont m_font;
};

#endif // LAPCOUNTMENU_HPP
