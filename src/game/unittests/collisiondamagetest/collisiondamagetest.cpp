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

#include "collisiondamagetest.hpp"
#include "../../collisiondamage.hpp"

void CollisionDamageTest::testScrapesCauseNoDamage()
{
    QCOMPARE(CollisionDamage::damage(0.0f, true), 0.0f);
    QCOMPARE(CollisionDamage::damage(0.5f, true), 0.0f);
    QCOMPARE(CollisionDamage::damage(CollisionDamage::minImpulse, true), 0.0f);
}

void CollisionDamageTest::testRepeatedScrapingCausesNoDamage()
{
    // E.g. a car sliding along a wall gets a small impulse on every step
    float total = 0;
    for (int step = 0; step < 60 * 60; step++)
    {
        total += CollisionDamage::damage(0.3f, true);
    }

    QCOMPARE(total, 0.0f);
}

void CollisionDamageTest::testDamageGrowsWithTheImpulse()
{
    float previous = 0;
    for (float impulse = CollisionDamage::minImpulse + 0.5f; impulse < 25; impulse += 0.5f)
    {
        const float damage = CollisionDamage::damage(impulse, true);
        QVERIFY(damage > previous);
        previous = damage;
    }
}

void CollisionDamageTest::testHardCrashIsDevastating()
{
    // A head-on crash at about 100 km/h takes a good part of the damage capacity of 100,
    // and two crashes at full speed wreck the car.
    QVERIFY(CollisionDamage::damage(10, true) > 10);
    QVERIFY(CollisionDamage::damage(25, true) > 50);
}

void CollisionDamageTest::testComputerPlayersTakeLessDamage()
{
    QVERIFY(CollisionDamage::damage(10, false) < CollisionDamage::damage(10, true));
    QVERIFY(CollisionDamage::damage(10, false) > 0);
}

QTEST_GUILESS_MAIN(CollisionDamageTest)
