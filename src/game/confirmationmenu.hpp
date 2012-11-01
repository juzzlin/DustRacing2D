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

#ifndef CONFIRMATIONMENU_HPP
#define CONFIRMATIONMENU_HPP

#include "surfacemenu.hpp"

namespace MTFH {
class MenuItem;
class MenuItemAction;
}

//! Yes/No menu.
class ConfirmationMenu : public SurfaceMenu
{
public:

    //! Constructor.
    ConfirmationMenu(std::string id, int width, int height);

    void setAcceptAction(MTFH::MenuItemAction & action);

    void setCancelAction(MTFH::MenuItemAction & action);

    //! \reimp
    virtual void selectCurrentItem();

private:

    MTFH::MenuItem * m_acceptItem;
    MTFH::MenuItem * m_cancelItem;
};

#endif // CONFIRMATIONMENU_HPP
