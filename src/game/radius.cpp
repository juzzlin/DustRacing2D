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

#include "radius.hpp"

#include <cmath>
#include <limits>

float Radius::calculate(const Radius::CirclePoints & cp)
{
    const float & bx = cp.x1;
    const float & by = cp.y1;
    const float & cx = cp.x2;
    const float & cy = cp.y2;
    const float & dx = cp.x3;
    const float & dy = cp.y3;

    const float tp = cx * cx + cy * cy;
    const float bc = (bx * bx + by * by - tp) * 0.5f;
    const float cd = (tp - dx * dx - dy * dy) * 0.5f;

    float dt = (bx - cx) * (cy - dy) - (cx - dx) * (by - cy);

    if (std::fabs(dt) < std::numeric_limits<float>::min())
    {
        return 0;
    }

    dt = 1.0f / dt;

    const float centerX = (bc * (cy - dy) - cd * (by - cy)) * dt;
    const float centerY = (cd * (bx - cx) - bc * (cx - dx)) * dt;

    // Min-max approximation of sqrt(x * x + y * y).
    // Original: return std::sqrt((centerX - bx) * (centerX - bx) +
    //                            (centerY - by) * (centerY - by));

    const float absX = std::fabs(centerX - bx);
    const float absY = std::fabs(centerY - by);

    if (absX > absY)
    {
        return absX + absY * 0.5f;
    }
    else
    {
        return absY + absX * 0.5f;
    }
}
