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

#include "MCTrigonomTest.hpp"

#include "../../Core/mcmathutil.hh"
#include "../../Core/mctrigonom.hh"
#include "../../Core/mcvector2d.hh"

#include <cmath>

//! The lookup table is built at 0.1 degree resolution, so this is the accuracy we can ask for.
static const float TABLE_TOLERANCE = 0.001f;

MCTrigonomTest::MCTrigonomTest()
{
}

void MCTrigonomTest::testDegToRadRoundTrip()
{
    for (float angle = -720.0f; angle <= 720.0f; angle += 17.0f)
    {
        QVERIFY(std::fabs(MCTrigonom::radToDeg(MCTrigonom::degToRad(angle)) - angle) < 0.001f);
    }

    QVERIFY(std::fabs(MCTrigonom::degToRad(180.0f) - float(M_PI)) < 0.0001f);
    QVERIFY(std::fabs(MCTrigonom::radToDeg(float(M_PI)) - 180.0f) < 0.001f);
}

void MCTrigonomTest::testCardinalAnglesAreExact()
{
    QVERIFY(qFuzzyCompare(MCTrigonom::cos(0.0f), 1.0f));
    QVERIFY(qFuzzyCompare(MCTrigonom::sin(90.0f), 1.0f));
    QVERIFY(qFuzzyCompare(MCTrigonom::cos(180.0f), -1.0f));
    QVERIFY(qFuzzyCompare(MCTrigonom::sin(270.0f), -1.0f));

    // The lookup table is filled with
    //
    //     sin(degToRad(i / SCALE - LUT_SIZE / 2))
    //
    // where "LUT_SIZE / 2" is the integer 3600 while the intended offset is
    // "LUT_SIZE / 2 / SCALE" == 360 degrees. The two differ by 3240 degrees, which is
    // exactly nine full turns, so periodicity hides the mistake and the table still holds
    // the right values -- but they are evaluated ~56 radians away from zero, where float
    // arguments have lost most of their precision. sin(0) comes out as ~-4e-6 instead of 0.
    //
    // That is still small enough to slip past qFuzzyIsNull(), so assert a tight bound:
    // a table built at the intended angles hits these two zeroes to within 1e-7.
    const float exactTolerance = 1.0e-7f;

    QEXPECT_FAIL("", "The sine table is built from angles offset by nine full turns, costing ~5 digits of precision", Continue);
    QVERIFY(std::fabs(MCTrigonom::sin(0.0f)) < exactTolerance);

    QEXPECT_FAIL("", "The cosine table is built from angles offset by nine full turns, costing ~5 digits of precision", Continue);
    QVERIFY(std::fabs(MCTrigonom::cos(90.0f)) < exactTolerance);
}

void MCTrigonomTest::testAgainstStdLib_data()
{
    QTest::addColumn<float>("angle");

    for (int angle = -350; angle <= 350; angle += 10)
    {
        QTest::newRow(qPrintable(QString::number(angle))) << float(angle);
    }
}

void MCTrigonomTest::testAgainstStdLib()
{
    QFETCH(float, angle);

    const float radians = angle * float(M_PI) / 180.0f;

    QVERIFY(std::fabs(MCTrigonom::sin(angle) - std::sin(radians)) < TABLE_TOLERANCE);
    QVERIFY(std::fabs(MCTrigonom::cos(angle) - std::cos(radians)) < TABLE_TOLERANCE);
}

void MCTrigonomTest::testPythagoreanIdentity()
{
    // MCObject::direction() builds a heading vector out of these two. If the identity
    // does not hold, every heading is subtly non-unit.
    for (float angle = -359.0f; angle < 360.0f; angle += 7.0f)
    {
        const float s = MCTrigonom::sin(angle);
        const float c = MCTrigonom::cos(angle);
        QVERIFY(std::fabs(s * s + c * c - 1.0f) < TABLE_TOLERANCE);
    }
}

void MCTrigonomTest::testAnglesOutsideTheTable()
{
    // Angles beyond +-360 degrees are normalized before the lookup. Car angles grow
    // without bound while spinning, so this path is hit in practice.
    QVERIFY(std::fabs(MCTrigonom::sin(370.0f) - MCTrigonom::sin(10.0f)) < TABLE_TOLERANCE);
    QVERIFY(std::fabs(MCTrigonom::cos(370.0f) - MCTrigonom::cos(10.0f)) < TABLE_TOLERANCE);

    QVERIFY(std::fabs(MCTrigonom::sin(-370.0f) - MCTrigonom::sin(-10.0f)) < TABLE_TOLERANCE);
    QVERIFY(std::fabs(MCTrigonom::cos(-370.0f) - MCTrigonom::cos(-10.0f)) < TABLE_TOLERANCE);

    QVERIFY(std::fabs(MCTrigonom::sin(3690.0f) - MCTrigonom::sin(90.0f)) < TABLE_TOLERANCE);
    QVERIFY(std::fabs(MCTrigonom::cos(-3690.0f) - MCTrigonom::cos(-90.0f)) < TABLE_TOLERANCE);

    // Exactly +-360 degrees is not normalized and falls through to std::sin / std::cos.
    QVERIFY(std::fabs(MCTrigonom::cos(360.0f) - 1.0f) < TABLE_TOLERANCE);
    QVERIFY(std::fabs(MCTrigonom::cos(-360.0f) - 1.0f) < TABLE_TOLERANCE);
}

void MCTrigonomTest::testRotatedVector()
{
    // Used by Car to place the rear tyres and by MCObject to place child objects.
    const MCVector2dF rotated = MCMathUtil::rotatedVector(MCVector2dF(1, 0), 90);
    QVERIFY(std::fabs(rotated.i()) < TABLE_TOLERANCE);
    QVERIFY(std::fabs(rotated.j() - 1.0f) < TABLE_TOLERANCE);

    // Rotating by 360 degrees in 10 degree increments must return the original vector.
    MCVector2dF accumulated(3, 4);
    for (int i = 0; i < 36; i++)
    {
        accumulated = MCMathUtil::rotatedVector(accumulated, 10);
    }

    QVERIFY(std::fabs(accumulated.i() - 3.0f) < 0.01f);
    QVERIFY(std::fabs(accumulated.j() - 4.0f) < 0.01f);

    // Rotation preserves the length.
    QVERIFY(std::fabs(MCMathUtil::rotatedVector(MCVector2dF(3, 4), 37).length() - 5.0f) < TABLE_TOLERANCE);
}

QTEST_GUILESS_MAIN(MCTrigonomTest)
