// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include <QString>

//! Config data for editor and for the game.
class Config
{
public:

    //! Common config data.
    class Common
    {
    public:

        //! The base data path.
        static QString dataPath;

        //! "Company" name used in QSettings.
        static QString QSETTINGS_COMPANY_NAME;

        //! Path used to search "3rd party" race tracks under the home dir:
        //! ~/TRACK_SEARCH_PATH/
        static QString TRACK_SEARCH_PATH;
    };

    //! Editor-specific config data.
    class Editor
    {
    public:

        static QString EDITOR_NAME;
        static QString EDITOR_VERSION;
        static QString MODEL_CONFIG_FILE_NAME;
        static QString QSETTINGS_SOFTWARE_NAME;

        //! Path to the "select" icon.
        static QString SELECT_PATH;

        //! Path to the "erase" icon.
        static QString ERASE_PATH;

        //! Path to the "clear" tile.
        static QString CLEAR_PATH;
    };

    //! Game-specific config data.
    class Game
    {
    public:

        static QString GAME_NAME;
        static QString GAME_VERSION;
        static QString QSETTINGS_SOFTWARE_NAME;

        //! Default window width.
        static const unsigned int WINDOW_WIDTH;

        //! Default window height.
        static const unsigned int WINDOW_HEIGHT;
    };
};
