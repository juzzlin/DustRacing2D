// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2009 Jussi Lind <jussi.lind@iki.fi>
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

//! Miscellaneous math utilities.
class MCMathUtil
{
    enum SIGN {NEG=0, POS, ZERO};

public:

    //! Returns true if x == y. Can be used with MCFloats / MCFloats.
    template <typename T>
    inline static bool equals(T x, T y);

    //! Returns the minimum distance between a point and a vector.
    //! Uses a fast approximation.
    //! \param p Point to be tested
    //! \param v Vector that defines the line
    template <typename T>
    inline static T distanceFromVector(const MCVector2d<T> & p, const MCVector2d<T> & v);

    //! Returns vector a projected on vector b.
    template <typename T>
    inline static MCVector2d<T> projection(
        const MCVector2d<T> & a, const MCVector2d<T> & b);

    //! Returns the sign of x.
    template <typename T>
    inline static MCMathUtil::SIGN sign(T x);

    //! Returns the absolute of x.
    template <typename T>
    inline static T abs(T x);
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
T MCMathUtil::distanceFromVector(const MCVector2d<T> & p, const MCVector2d<T> & v)
{
    return abs(p.dot(MCVector2d<MCFloat>(-v.j(), v.i()).normalizedFast()));
}

template <typename T>
MCVector2d<T> MCMathUtil::projection(
    const MCVector2d<T> & a, const MCVector2d<T> & b)
{
    return b * a.dot(b) / b.lengthSquared();
}

template <typename T>
MCMathUtil::SIGN MCMathUtil::sign(T x)
{
    if (x < 0)
    {
        return MCMathUtil::NEG;
    }
    else if (x > 0)
    {
        return MCMathUtil::POS;
    }
    else
    {
        return MCMathUtil::ZERO;
    }
}

template <typename T>
T MCMathUtil::abs(T x)
{
    return x < 0 ? -x : x;
}

#endif // MCMATHUTIL_HH

