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

#ifndef CONFIRMATIONMENU_HPP
#define CONFIRMATIONMENU_HPP

#include <functional>
#include <memory>

#include "surfacemenu.hpp"

#include <MenuItem>
#include <MenuItemAction>

//! Yes/No menu.
class ConfirmationMenu : public SurfaceMenu
{
public:
    //! Constructor.
    ConfirmationMenu(std::string id, int width, int height);

    void setAcceptAction(MTFH::MenuItemActionPtr action);

    void setAcceptAction(std::function<void()> actionFunction);

    void setCancelAction(MTFH::MenuItemActionPtr action);

    void setCancelAction(std::function<void()> actionFunction);

    void setText(std::wstring text);

    //! \reimp
    virtual void selectCurrentItem();

    //! \reimp
    virtual void render();

private:
    MTFH::MenuItemPtr m_acceptItem;
    MTFH::MenuItemPtr m_cancelItem;

    std::wstring m_text;
};

using ConfirmationMenuPtr = std::shared_ptr<ConfirmationMenu>;

#endif // CONFIRMATIONMENU_HPP
