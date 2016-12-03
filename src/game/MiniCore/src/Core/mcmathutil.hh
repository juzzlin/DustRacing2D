// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA.
//

#ifndef MCMATHUTIL_HH
#define MCMATHUTIL_HH

#include <limits>

using std::numeric_limits;

#include <cmath>

using std::abs;
using std::sqrt;

#include <algorithm>

using std::max;

#include "mcvector2d.hh"
#include "mcsegment.hh"

//! Miscellaneous math utilities.
class MCMathUtil
{
public:

    enum class Sign
    {
        Negative,
        Positive,
        Zero
    };

    //! \returns the absolute of x.
    template <typename T>
    inline static T abs(T x);

    //! \returns true if x == y. Can be used with integers / floats.
    template <typename T>
    inline static bool equals(T x, T y);

    //! \returns the sign of x.
    template <typename T>
    inline static MCMathUtil::Sign sign(T x);

    //! \returns true if the given segments cross.
    static bool crosses(const MCSegmentF & a, const MCSegmentF & b);

    /*! \returns the minimum distance between a point and a vector.
     *  Uses a fast approximation.
     *  \param p Point to be tested
     *  \param v Vector that defines the line */
    static MCFloat distanceFromVector(const MCVector2dF & p, const MCVector2dF & v);

    //! Return X-coordinate of the given point rotated by given angle.
    static MCFloat rotatedX(MCFloat x0, MCFloat y0, MCFloat angle);

    //! Return Y-coordinate of the given point rotated by given angle.
    static MCFloat rotatedY(MCFloat x0, MCFloat y0, MCFloat angle);

    //! Rotate given vector v0 by given angle and store result to v1.
    static void rotateVector(const MCVector2dF & v0, MCVector2dF & v1, MCFloat angle);

    //! Rotate given vector v0 by given angle.
    static MCVector2dF rotatedVector(const MCVector2dF & v0, MCFloat angle);
};

template <typename T>
bool MCMathUtil::equals(T x, T y)
{
    if (numeric_limits<T>::is_exact)
    {
        return x == y;
    }
    else
    {
        const T m = max(abs(x), abs(y));
        if (m > 0)
        {
            return abs(x - y) / m <= numeric_limits<T>::epsilon();
        }
        else
        {
            return true;
        }
    }
}

template <typename T>
MCMathUtil::Sign MCMathUtil::sign(T x)
{
    if (x < 0)
    {
        return MCMathUtil::Sign::Negative;
    }
    else if (x > 0)
    {
        return MCMathUtil::Sign::Positive;
    }
    else
    {
        return MCMathUtil::Sign::Zero;
    }
}

template <typename T>
T MCMathUtil::abs(T x)
{
    return x < 0 ? -x : x;
}

#endif // MCMATHUTIL_HH

