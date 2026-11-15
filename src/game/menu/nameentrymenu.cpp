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

#include "nameentrymenu.hpp"

#include "../playername.hpp"
#include "../settings.hpp"

#include <MCTextureFont>
#include <MCTextureText>

#include <QObject> // For QObject::tr()

namespace {

const float titleSize { 24 };

const float nameSize { 40 };

const float hintSize { 16 };

const int cursorBlinkPeriod { 1000 }; // Milliseconds

const MCGLColor titleColor { 0.25f, 0.75f, 1.0f, 1.0f };

const MCGLColor nameColor { 1.0f, 1.0f, 1.0f, 1.0f };

const MCGLColor hintColor { 0.75f, 0.75f, 0.75f, 1.0f };

} // namespace

NameEntryMenu::NameEntryMenu(std::string id, int width, int height)
  : SurfaceMenu { "settingsBack", id, width, height, Menu::Style::VerticalList }
{
}

void NameEntryMenu::setPlayer(size_t player)
{
    m_player = player;
}

void NameEntryMenu::setSavedCallback(SavedCallback callback)
{
    m_savedCallback = callback;
}

void NameEntryMenu::pushEnter()
{
    SurfaceMenu::pushEnter();

    m_name = Settings::instance().loadPlayerName(m_player);
    m_cursor = static_cast<int>(m_name.length());
    restartCursorBlink();
}

bool NameEntryMenu::isDrawable(QChar character) const
{
    return font().hasGlyph(static_cast<wchar_t>(character.unicode()));
}

void NameEntryMenu::restartCursorBlink()
{
    m_blinkTime = 0;
}

bool NameEntryMenu::wantsTextInput() const
{
    return true;
}

void NameEntryMenu::insertText(const std::wstring & text)
{
    const auto isDrawable { [this](QChar character) { return this->isDrawable(character); } };
    for (auto && character : QString::fromStdWString(text))
    {
        if (m_name.length() < PlayerName::maxLength && PlayerName::isAllowed(character, isDrawable))
        {
            m_name.insert(m_cursor, character);
            m_cursor++;
        }
    }

    restartCursorBlink();
}

void NameEntryMenu::eraseCharacter()
{
    if (m_cursor > 0)
    {
        m_cursor--;
        m_name.remove(m_cursor, 1);
    }

    restartCursorBlink();
}

void NameEntryMenu::left()
{
    m_cursor = std::max(m_cursor - 1, 0);
    restartCursorBlink();
}

void NameEntryMenu::right()
{
    m_cursor = std::min(m_cursor + 1, static_cast<int>(m_name.length()));
    restartCursorBlink();
}

void NameEntryMenu::selectCurrentItem()
{
    const auto isDrawable { [this](QChar character) { return this->isDrawable(character); } };
    const QString name { PlayerName::sanitize(m_name, Settings::defaultPlayerName(m_player), isDrawable) };
    Settings::instance().savePlayerName(m_player, name);

    if (m_savedCallback)
    {
        m_savedCallback(m_player, name);
    }

    exit();
}

void NameEntryMenu::stepTime(int msecs)
{
    SurfaceMenu::stepTime(msecs);

    m_blinkTime = (m_blinkTime + msecs) % cursorBlinkPeriod;
}

float NameEntryMenu::textWidth(const QString & text, float size) const
{
    MCTextureText textureText { text.toStdWString() };
    textureText.setGlyphSize(size, size);
    return textureText.width(font());
}

void NameEntryMenu::renderText(const QString & text, float size, const MCGLColor & color, float x, float y)
{
    MCTextureText textureText { text.toStdWString() };
    textureText.setGlyphSize(size, size);
    textureText.setColor(color);
    textureText.setShadowOffset(2, -2);
    textureText.render(x, y, nullptr, font());
}

void NameEntryMenu::render()
{
    SurfaceMenu::render();

    const float centerX { x() + width() / 2 };
    const float centerY { y() + height() / 2 };

    const QString title { QObject::tr("Player %1 name").arg(m_player + 1).toUpper() };
    renderText(title, titleSize, titleColor, centerX - textWidth(title, titleSize) / 2, centerY + nameSize * 2);

    const float nameX { centerX - textWidth(m_name, nameSize) / 2 };
    renderText(m_name, nameSize, nameColor, nameX, centerY);

    if (m_blinkTime < cursorBlinkPeriod / 2)
    {
        const QString cursor { "|" };
        const float cursorX { nameX + textWidth(m_name.left(m_cursor), nameSize) - textWidth(cursor, nameSize) / 2 };
        renderText(cursor, nameSize, titleColor, cursorX, centerY);
    }

    const QString counter { QString { "%1/%2" }.arg(m_name.length()).arg(PlayerName::maxLength) };
    renderText(counter, hintSize, hintColor, centerX - textWidth(counter, hintSize) / 2, centerY - nameSize);

    const QString hint { QObject::tr("Enter: Save   Esc: Cancel").toUpper() };
    renderText(hint, hintSize, hintColor, centerX - textWidth(hint, hintSize) / 2, centerY - nameSize * 2);
}
