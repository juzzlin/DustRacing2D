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

#include "playernametest.hpp"
#include "../../playername.hpp"

namespace {

const QString fallback { "Player 1" };

const PlayerName::DrawableCheck allDrawable { [](QChar) { return true; } };

} // namespace

void PlayerNameTest::testValidNameIsKeptAsIs()
{
    QCOMPARE(PlayerName::sanitize("Jussi", fallback, allDrawable), QString { "Jussi" });
}

void PlayerNameTest::testNonLatinNameIsKept()
{
    QCOMPARE(PlayerName::sanitize(QString::fromUtf8("Jyrkkä Öljy"), fallback, allDrawable), QString::fromUtf8("Jyrkkä Öljy"));
}

void PlayerNameTest::testWhitespaceIsCollapsed()
{
    QCOMPARE(PlayerName::sanitize("  Big \t  Ben  ", fallback, allDrawable), QString { "Big Ben" });
}

void PlayerNameTest::testNameIsTruncatedToMaxLength()
{
    const QString name { PlayerName::sanitize("Abcdefghijklmnopqrstuvwxyz", fallback, allDrawable) };
    QCOMPARE(name.length(), PlayerName::maxLength);
    QCOMPARE(name, QString { "Abcdefghijkl" });
}

void PlayerNameTest::testTruncationDoesNotLeaveTrailingSpace()
{
    QCOMPARE(PlayerName::sanitize("Abcdefghijk Lmn", fallback, allDrawable), QString { "Abcdefghijk" });
}

void PlayerNameTest::testControlCharactersAreRemoved()
{
    QCOMPARE(PlayerName::sanitize(QString { "Ju" } + QChar { 0x07 } + "ssi", fallback, allDrawable), QString { "Jussi" });
}

void PlayerNameTest::testUndrawableCharactersAreRemoved()
{
    const PlayerName::DrawableCheck onlyAscii { [](QChar character) { return character.unicode() < 128; } };
    QCOMPARE(PlayerName::sanitize(QString::fromUtf8("Ann漢a"), fallback, onlyAscii), QString { "Anna" });
}

void PlayerNameTest::testEmptyNameFallsBack()
{
    QCOMPARE(PlayerName::sanitize("", fallback, allDrawable), fallback);
}

void PlayerNameTest::testWhitespaceOnlyNameFallsBack()
{
    QCOMPARE(PlayerName::sanitize("   ", fallback, allDrawable), fallback);
}

QTEST_GUILESS_MAIN(PlayerNameTest)
