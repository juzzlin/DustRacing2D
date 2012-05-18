// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

//! Config data for editor and for the game.
class Config
{
public:

    //! Common config data.
    class Common
    {
    public:

        //! The base data path.
        static const char * dataPath;
    };

    //! Editor-specific config data.
    class Editor
    {
    public:

        static const char * EDITOR_NAME;
        static const char * EDITOR_VERSION;
        static const char * QSETTINGS_COMPANY_NAME;
        static const char * QSETTINGS_SOFTWARE_NAME;
        static const char * MODEL_CONFIG_FILE_NAME;

        //! Path to the "select" icon.
        static const char * SELECT_PATH;

        //! Path to the "clear" tile.
        static const char * CLEAR_PATH;
    };

    //! Game-specific config data.
    class Game
    {
    public:

        static const char * GAME_NAME;
        static const char * GAME_VERSION;
        static const char * QSETTINGS_COMPANY_NAME;
        static const char * QSETTINGS_SOFTWARE_NAME;

        //! Window width.
        static const unsigned int WINDOW_WIDTH = 800;

        //! Window height.
        static const unsigned int WINDOW_HEIGHT = 600;
    };
};
