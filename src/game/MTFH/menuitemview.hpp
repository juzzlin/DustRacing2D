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

#ifndef MENUITEMVIEW_HPP
#define MENUITEMVIEW_HPP

#include <memory>

namespace MTFH {

class MenuItem;

//! Base class for menu item views.
class MenuItemView
{
public:
    //! Constructor.
    MenuItemView(MenuItem & owner);

    //! Destructor.
    virtual ~MenuItemView();

    //! Render the view.
    virtual void render(float x, float y) = 0;

    //! \return the owner item.
    MenuItem & owner() const;

    //! Re-imp for animations etc.
    virtual void stepTime(int msecs);

private:
    MenuItem & m_owner;
};

typedef std::shared_ptr<MenuItemView> MenuItemViewPtr;

} // namespace MTFH

#endif // MENUITEMVIEW_HPP
