// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "MCVectorTest.hpp"

#include "../../Core/mcvector2d.hh"
#include "../../Core/mcvector3d.hh"

#include <cmath>

MCVectorTest::MCVectorTest()
{
}

void MCVectorTest::testLength()
{
    QVERIFY(qFuzzyCompare(MCVector2dF(3, 4).length(), 5.0f));
    QVERIFY(qFuzzyCompare(MCVector2dF(3, 4).lengthSquared(), 25.0f));
    QVERIFY(qFuzzyCompare(MCVector3dF(1, 2, 2).length(), 3.0f));
    QVERIFY(qFuzzyIsNull(MCVector2dF().length()));
}

void MCVectorTest::testLengthFastIsIsotropic()
{
    // lengthFast() is the "max + min / 2" approximation. It is exact on the axes..
    QVERIFY(qFuzzyCompare(MCVector2dF(5, 0).lengthFast(), 5.0f));
    QVERIFY(qFuzzyCompare(MCVector2dF(0, -5).lengthFast(), 5.0f));

    // ..but over-estimates a diagonal by ~6 %. Every consumer of lengthFast() (speeds,
    // clamps, sleep limits, tire grip) therefore gets a direction dependent answer.
    const MCVector2dF diagonal(1, 1);
    QVERIFY(diagonal.lengthFast() > diagonal.length());

    QEXPECT_FAIL("", "lengthFast() over-estimates diagonal vectors by up to ~6 %", Continue);
    QVERIFY(std::fabs(diagonal.lengthFast() - diagonal.length()) < 0.01f);
}

void MCVectorTest::testNormalizeZeroVectorIsSafe()
{
    // Force generators normalize the velocity of objects that may be standing still.
    // Both the in-place and the value returning variants must leave a zero vector alone
    // rather than producing NaNs.
    MCVector2dF zero2d;
    zero2d.normalize();
    QVERIFY(qFuzzyIsNull(zero2d.length()));

    zero2d.normalizeFast();
    QVERIFY(qFuzzyIsNull(zero2d.length()));

    QVERIFY(qFuzzyIsNull(MCVector2dF().normalized().length()));
    QVERIFY(qFuzzyIsNull(MCVector2dF().normalizedFast().length()));

    MCVector3dF zero3d;
    zero3d.normalize();
    QVERIFY(qFuzzyIsNull(zero3d.length()));

    QVERIFY(qFuzzyIsNull(MCVector3dF().normalized().length()));
    QVERIFY(qFuzzyIsNull(MCVector3dF().normalizedFast().length()));
}

void MCVectorTest::testNormalizedFastIsAUnitVector()
{
    QVERIFY(qFuzzyCompare(MCVector2dF(7, 0).normalizedFast().length(), 1.0f));

    // normalizedFast() divides by the approximate length, so a diagonal "unit" vector
    // comes out roughly 6 % too short.
    QEXPECT_FAIL("", "normalizedFast() does not produce a unit vector for diagonal input", Continue);
    QVERIFY(qFuzzyCompare(MCVector2dF(1, 1).normalizedFast().length(), 1.0f));
}

void MCVectorTest::testClamp()
{
    MCVector2dF longVector(30, 40);
    longVector.clamp(5);
    QVERIFY(qFuzzyCompare(longVector.length(), 5.0f));

    MCVector2dF shortVector(3, 4);
    shortVector.clamp(10);
    QVERIFY(qFuzzyCompare(shortVector.length(), 5.0f)); // Untouched
}

void MCVectorTest::testClampFastIsIsotropic()
{
    MCVector2dF axisAligned(10, 0);
    axisAligned.clampFast(2);
    QVERIFY(qFuzzyCompare(axisAligned.length(), 2.0f));

    MCVector2dF diagonal(10, 10);
    diagonal.clampFast(2);

    QEXPECT_FAIL("", "clampFast() clamps diagonal vectors ~6 % too short", Continue);
    QVERIFY(qFuzzyCompare(diagonal.length(), 2.0f));
}

void MCVectorTest::testDotProduct()
{
    QVERIFY(qFuzzyCompare(MCVector2dF(1, 0).dot(MCVector2dF(3, 4)), 3.0f));
    QVERIFY(qFuzzyIsNull(MCVector2dF(1, 0).dot(MCVector2dF(0, 4))));
    QVERIFY(qFuzzyCompare(MCVector3dF(1, 2, 3).dot(MCVector3dF(4, 5, 6)), 32.0f));
}

void MCVectorTest::testCrossProduct()
{
    // The impulse and torque code relies on the k-component of the cross product.
    const MCVector3dF cross = MCVector3dF(1, 0, 0) % MCVector3dF(0, 1, 0);
    QVERIFY(qFuzzyIsNull(cross.i()));
    QVERIFY(qFuzzyIsNull(cross.j()));
    QVERIFY(qFuzzyCompare(cross.k(), 1.0f));

    // Anti-symmetry
    QVERIFY(qFuzzyCompare((MCVector3dF(0, 1, 0) % MCVector3dF(1, 0, 0)).k(), -1.0f));

    // Parallel vectors have a zero cross product
    QVERIFY(qFuzzyIsNull((MCVector3dF(2, 2, 0) % MCVector3dF(4, 4, 0)).k()));
}

void MCVectorTest::testProjection()
{
    // Tire uses this to split the velocity into a lateral component.
    const MCVector2dF projected = MCVector2dF::projection(MCVector2dF(3, 4), MCVector2dF(1, 0));
    QVERIFY(qFuzzyCompare(projected.i(), 3.0f));
    QVERIFY(qFuzzyIsNull(projected.j()));

    // Projecting onto a longer axis must not scale the result.
    const MCVector2dF onLongAxis = MCVector2dF::projection(MCVector2dF(3, 4), MCVector2dF(10, 0));
    QVERIFY(qFuzzyCompare(onLongAxis.i(), 3.0f));

    // A velocity perpendicular to the axis projects to zero.
    QVERIFY(qFuzzyIsNull(MCVector2dF::projection(MCVector2dF(0, 4), MCVector2dF(1, 0)).length()));
}

QTEST_GUILESS_MAIN(MCVectorTest)
