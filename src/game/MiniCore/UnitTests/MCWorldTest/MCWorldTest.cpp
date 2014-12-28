// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#include "MCWorldTest.hpp"
#include "../../Core/mcworld.hh"
#include "../../Core/mcobject.hh"

MCWorldTest::MCWorldTest()
{
}

void MCWorldTest::testAddToWorld()
{
    MCWorld world;
    MCObject object("test");
    QVERIFY(object.index() == -1);
    world.addObject(object);
    QVERIFY(object.index() >= 0);

    world.removeObject(object); // Lazy removal
    QVERIFY(object.index() >= 0);
    world.stepTime(1);
    QVERIFY(object.index() == -1);

    world.addObject(object);
    QVERIFY(object.index() >= 0);

    world.removeObjectNow(object); // Immediate removal
    QVERIFY(object.index() == -1);
}

void MCWorldTest::testSetDimensions()
{
    const MCFloat minX = 0;
    const MCFloat maxX = 1024;
    const MCFloat minY = 0;
    const MCFloat maxY = 768;
    const MCFloat minZ = 0;
    const MCFloat maxZ = 1000;
    const MCFloat metersPerPixel = 0.4;

    MCWorld world;
    world.setDimensions(minX, maxX, minY, maxY, minZ, maxZ, metersPerPixel);

    QVERIFY(qFuzzyCompare(world.minX(), minX));
    QVERIFY(qFuzzyCompare(world.maxX(), maxX));
    QVERIFY(qFuzzyCompare(world.minY(), minY));
    QVERIFY(qFuzzyCompare(world.maxY(), maxY));
    QVERIFY(qFuzzyCompare(world.minZ(), minZ));
    QVERIFY(qFuzzyCompare(world.maxZ(), maxZ));

    QVERIFY(qFuzzyCompare(world.metersPerPixel(), metersPerPixel));
}

QTEST_MAIN(MCWorldTest)
