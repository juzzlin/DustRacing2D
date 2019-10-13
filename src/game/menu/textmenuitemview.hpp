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

#ifndef TEXTMENUITEMVIEW_HPP
#define TEXTMENUITEMVIEW_HPP

#include <MenuItemView>

namespace MTFH {
class MenuItem;
}

//! Base class for menu item views.
class TextMenuItemView : public MTFH::MenuItemView
{
public:
    //! Constructor.
    TextMenuItemView(float textSize, MTFH::MenuItem & owner);

    //! Destructor.
    virtual ~TextMenuItemView();

    //! \reimp
    virtual void render(float x, float y) override;

    //! \reimp
    virtual void stepTime(int msecs) override;

    //! Set the text size.
    virtual void setTextSize(float size);

private:
    float m_textSize;

    float m_angle;
};

#endif // TEXTMENUITEMVIEW_HPP
