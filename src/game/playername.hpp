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

#ifndef PLAYERNAME_HPP
#define PLAYERNAME_HPP

#include <QString>

#include <cstddef>
#include <functional>

namespace PlayerName {

//! Maximum length of a player name in characters.
constexpr int maxLength { 12 };

//! Tells if the given character can be drawn with the game font.
using DrawableCheck = std::function<bool(QChar)>;

/*! \return true if the character is allowed in a name: printable and drawable. */
bool isAllowed(QChar character, const DrawableCheck & isDrawable);

/*! Cleans up a name entered by the user: removes characters that aren't allowed,
 *  collapses whitespace and limits the length.
 *  \return The cleaned name, or the fallback if nothing was left. */
QString sanitize(const QString & name, const QString & fallback, const DrawableCheck & isDrawable);

} // namespace PlayerName

#endif // PLAYERNAME_HPP
