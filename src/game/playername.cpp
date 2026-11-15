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

#include "playername.hpp"

namespace PlayerName {

bool isAllowed(QChar character, const DrawableCheck & isDrawable)
{
    return character.isPrint() && isDrawable(character);
}

QString sanitize(const QString & name, const QString & fallback, const DrawableCheck & isDrawable)
{
    QString allowed;
    for (auto && character : name)
    {
        if (character.isSpace())
        {
            allowed += QChar { ' ' };
        }
        else if (isAllowed(character, isDrawable))
        {
            allowed += character;
        }
    }

    const QString sanitized { allowed.simplified().left(maxLength).trimmed() };
    return sanitized.isEmpty() ? fallback : sanitized;
}

} // namespace PlayerName
