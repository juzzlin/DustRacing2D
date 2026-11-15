// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "denormalstest.hpp"
#include "../../denormals.hpp"

#include <limits>

void DenormalsTest::testDenormalResultsAreFlushedToZero()
{
#if defined(__SSE2__) || defined(__aarch64__)
    Denormals::disable();

    // Volatile, so that the compiler can't calculate the result at compile time
    volatile float smallest = std::numeric_limits<float>::min();
    volatile float divisor = 4.0f;
    const float result = smallest / divisor;
    QCOMPARE(result, 0.0f);
#else
    QSKIP("Denormals can't be disabled on this platform");
#endif
}

void DenormalsTest::testNormalNumbersAreUnaffected()
{
    Denormals::disable();

    volatile float a = 1.5f;
    volatile float b = 3.0f;
    QCOMPARE(a / b, 0.5f);

    volatile float smallest = std::numeric_limits<float>::min();
    QCOMPARE(smallest * 2.0f, std::numeric_limits<float>::min() * 2.0f);
}

QTEST_GUILESS_MAIN(DenormalsTest)
