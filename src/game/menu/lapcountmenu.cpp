// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#include "lapcountmenu.hpp"

#include "menuitem.hpp"
#include "menuitemview.hpp"
#include "menumanager.hpp"
#include "textmenuitemview.hpp"
#include "trackselectionmenu.hpp"

#include "game.hpp"

#include <MCAssetManager>
#include <MCTextureText>

#include "simple_logger.hpp"

static const QString LAP_COUNT_KEY(Settings::lapCountKey());

static const int ITEM_TEXT_SIZE = 40;

std::string LapCountMenu::MenuId = "lapCount";

LapCountMenu::LapCountMenu(int width, int height)
  : SurfaceMenu("trackSelectionBack", MenuId, width, height, Menu::Style::HorizontalList)
  , m_font(MCAssetManager::textureFontManager().font(Game::instance().fontName()))
{
    static int LAP_COUNTS[] = {
        1, 3, 5, 10, 20, 50, 100
    };

    const int numLapCounts = sizeof(LAP_COUNTS) / sizeof(int);
    const int itemHeight = height / (numLapCounts + 2);
    const int itemWidth = width / (numLapCounts + 2);
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    int selectedIndex = 0;
    for (int i = 0; i < numLapCounts; i++)
    {
        std::wstringstream itemText;
        itemText << LAP_COUNTS[i];

        const auto lapCountItem = std::make_shared<MenuItem>(itemWidth, itemHeight, itemText.str());
        lapCountItem->setView(std::make_shared<TextMenuItemView>(textSize, *lapCountItem));
        lapCountItem->setAction(
          [i]() {
              juzzlin::L().info() << LAP_COUNTS[i] << " laps selected.";
              Game::instance().setLapCount(LAP_COUNTS[i]);
              Settings::instance().saveValue(LAP_COUNT_KEY, LAP_COUNTS[i]);
              MenuManager::instance().pushMenu(TrackSelectionMenu::MenuId);
          });

        addItem(lapCountItem);

        if (Game::instance().lapCount() == LAP_COUNTS[i])
        {
            selectedIndex = lapCountItem->index();
        }
    }

    setCurrentIndex(selectedIndex);
}

void LapCountMenu::render()
{
    SurfaceMenu::render();

    MCTextureText text(QObject::tr("Choose lap count").toUpper().toStdWString());

    const int shadowY = -2;
    const int shadowX = 2;

    text.setGlyphSize(30, 30);
    text.setShadowOffset(shadowX, shadowY);
    text.render(x() + width() / 2 - text.width(m_font) / 2, y() + height() / 2 + text.height(m_font) * 2, nullptr, m_font);
}
