// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef SURFACEMENU_HPP
#define SURFACEMENU_HPP

#include <Menu>

class MCSurface;
class MCTextureFont;

//! Base class for menus with a MCSurface background.
class SurfaceMenu : public MTFH::Menu
{
public:

    //! Constructor.
    SurfaceMenu(
        std::string surfaceId, std::string id, int width, int height,
        Menu::Style style = Menu::Style::VerticalList,
        bool quitItem = true, bool prevItem = false, bool nextItem = false);

    //! Destructor.
    virtual ~SurfaceMenu() {}

    //! \reimp
    virtual void render() override;

protected:

    MCTextureFont & font() const;

private:

    MCSurface     & m_back;
    MCTextureFont & m_font;
};

#endif // SURFACEMENU_HPP
