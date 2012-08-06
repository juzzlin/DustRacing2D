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

#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "menu.hpp"

//! The main menu of the game.
class MainMenu : public Menu
{
public:

    //! Constructor.
    MainMenu(std::string id, int width, int height);

    //! \reimp
    virtual void render();

private:
};

#endif // MAINMENU_HPP
