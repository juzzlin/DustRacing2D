#include "lapcountmenu.hpp"

#include "menumanager.hpp"
#include "menuitem.hpp"
#include "menuitemview.hpp"
#include "textmenuitemview.hpp"
#include "trackselectionmenu.hpp"

#include "game.hpp"

#include <MCLogger>

static const QString LAP_COUNT_KEY(Settings::lapCountKey());

static const int ITEM_TEXT_SIZE  = 40;

std::string LapCountMenu::MenuId = "lapCount";

LapCountMenu::LapCountMenu(int width, int height)
    : SurfaceMenu("trackSelectionBack", MenuId, width, height, Menu::Style::HorizontalList)
{
    static int LAP_COUNTS[] =
    {
        1, 3, 5, 10, 20, 50, 100
    };

    const int numLapCounts = sizeof(LAP_COUNTS) / sizeof(int);
    const int itemHeight   = height / (numLapCounts + 2);
    const int itemWidth    = width  / (numLapCounts + 2);
    const int textSize     = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    int selectedIndex = 0;
    for (int i = 0; i < numLapCounts; i++)
    {
        std::wstringstream itemText;
        itemText << LAP_COUNTS[i];

        MenuItem * lapCountItem = new MenuItem(itemWidth, itemHeight, itemText.str());
        lapCountItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *lapCountItem)));
        lapCountItem->setAction(
            [i]()
            {
                MCLogger().info() << LAP_COUNTS[i] << " laps selected.";
                Game::instance().setLapCount(LAP_COUNTS[i]);
                Settings::instance().saveValue(LAP_COUNT_KEY, LAP_COUNTS[i]);
                MenuManager::instance().pushMenu(TrackSelectionMenu::MenuId);
            });

        addItem(MTFH::MenuItemPtr(lapCountItem));

        if (Game::instance().lapCount() == LAP_COUNTS[i])
        {
            selectedIndex = lapCountItem->index();
        }
    }

    setCurrentIndex(selectedIndex);
}
