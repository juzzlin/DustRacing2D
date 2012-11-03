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

#ifndef WALL_HPP
#define WALL_HPP

#include <MCObject>
#include <MCMacros>

class MCCamera;

//! Stationary wall object
class Wall : public MCObject
{
public:

    //! Constructor.
    Wall(MCSurface & surface, int x, int y, int wx, int wy, int h);

    //! Destructor.
    virtual ~Wall();

    //! Render wall.
    virtual void render(MCCamera * pCamera);

    //! Render wall shadows.
    virtual void renderShadow(MCCamera * pCamera);

private:

    //! Disable copy constructor
    DISABLE_COPY(Wall);

    //! Disable assignment
    DISABLE_ASSI(Wall);

    //! Width (x-axis)
    int wx;

    //! Width (y-axis)
    int wy;

    //! Width / 2 (x-axis)
    int wx2;

    //! Width / 2 (y-axis)
    int wy2;

    //! Height (z-axis)
    int h;
};

#endif // WALL_HPP
