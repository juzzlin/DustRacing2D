// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef LAYERS_HPP
#define LAYERS_HPP

//! Layers define the rendering order (stacking)
//! of different objects.
class Layers
{
public:

    enum Order
    {
        Ground      = 0,
        Meshes      = 1,
        Cars        = 2,
        Smoke       = 3,
        Walls       = 4,
        GrandStands = 5,
        Tree        = 10
    };
};

#endif // LAYERS_HPP
