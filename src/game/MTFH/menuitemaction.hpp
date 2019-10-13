// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MENUITEMACTION_HPP
#define MENUITEMACTION_HPP

#include <memory>

namespace MTFH {

//! Base class for menu item actions triggered when
//! the item gets selected.
class MenuItemAction
{
public:
    //! Constructor.
    MenuItemAction();

    //! Called on select.
    virtual void fire()
    {
        // Do nothing by default.
    }

    //! Destructor.
    virtual ~MenuItemAction();
};

typedef std::shared_ptr<MenuItemAction> MenuItemActionPtr;

} // namespace MTFH

#endif // MENUITEMACTION_HPP
