// This file is part of Dust Racing 2D.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
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

#include "gearboxtest.hpp"
#include "gearbox.hpp"

GearboxTest::GearboxTest()
{
}

void GearboxTest::testInit()
{
    Gearbox gb;

    QVERIFY(gb.gear() == Gearbox::Gear::Neutral);
}

void GearboxTest::testGearForward()
{
    Gearbox gb;

    gb.setAcceleratorEnabled(true);
    gb.update(1);

    QVERIFY(gb.gear() == Gearbox::Gear::Forward);
}

void GearboxTest::testGearReverse()
{
    Gearbox gb;

    gb.setAcceleratorEnabled(true);
    gb.update(1);

    QVERIFY(gb.gear() == Gearbox::Gear::Forward);

    gb.setAcceleratorEnabled(false);
    gb.setBrakeEnabled(true);

    gb.update(1);

    QVERIFY(gb.gear() == Gearbox::Gear::Forward);

    gb.update(0);

    for (int i = 0; i < 31; i++)
    {
        QVERIFY(gb.gear() == Gearbox::Gear::Stop);

        gb.update(0);
    }

    QVERIFY(gb.gear() == Gearbox::Gear::Reverse);

    gb.setBrakeEnabled(false);
    gb.setAcceleratorEnabled(false);
    gb.update(1);

    QVERIFY(gb.gear() == Gearbox::Gear::Neutral);

    gb.setBrakeEnabled(false);
    gb.setAcceleratorEnabled(true);
    gb.update(1);

    QVERIFY(gb.gear() == Gearbox::Gear::Forward);
}

void GearboxTest::testBothPressed()
{
    Gearbox gb;

    gb.setAcceleratorEnabled(true);
    gb.update(1);

    QVERIFY(gb.gear() == Gearbox::Gear::Forward);

    gb.setBrakeEnabled(true);
    gb.setAcceleratorEnabled(true);

    gb.update(1);

    QVERIFY(gb.gear() == Gearbox::Gear::Neutral);
}

QTEST_GUILESS_MAIN(GearboxTest)
