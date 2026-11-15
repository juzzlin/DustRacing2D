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

#ifndef NAMEENTRYMENU_HPP
#define NAMEENTRYMENU_HPP

#include "surfacemenu.hpp"

#include <MCGLColor>

#include <QString>

#include <functional>
#include <memory>

//! A menu with a text field for entering the name of a human player.
class NameEntryMenu : public SurfaceMenu
{
public:
    //! Called with the new name after it has been saved.
    using SavedCallback = std::function<void(size_t player, QString name)>;

    NameEntryMenu(std::string id, int width, int height);

    //! Selects the player whose name is edited. Must be called before entering the menu.
    void setPlayer(size_t player);

    void setSavedCallback(SavedCallback callback);

    //! \reimp
    void render() override;

    //! \reimp
    void stepTime(int msecs) override;

    //! \reimp
    void left() override;

    //! \reimp
    void right() override;

    //! \reimp
    void selectCurrentItem() override;

    //! \reimp
    bool wantsTextInput() const override;

    //! \reimp
    void insertText(const std::wstring & text) override;

    //! \reimp
    void eraseCharacter() override;

protected:
    //! \reimp
    void pushEnter() override;

private:
    bool isDrawable(QChar character) const;

    void restartCursorBlink();

    void renderText(const QString & text, float size, const MCGLColor & color, float x, float y);

    float textWidth(const QString & text, float size) const;

    size_t m_player { 0 };

    QString m_name;

    int m_cursor { 0 };

    int m_blinkTime { 0 };

    SavedCallback m_savedCallback;
};

using NameEntryMenuPtr = std::shared_ptr<NameEntryMenu>;

#endif // NAMEENTRYMENU_HPP
