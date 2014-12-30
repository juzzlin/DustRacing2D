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

#include "config.hpp"

// **** Common variables ****

// This is the main data path and should come as a define
// from the build system.
QString Config::Common::dataPath                = DATA_PATH;
QString Config::Common::QSETTINGS_COMPANY_NAME  = "dustrac";
QString Config::Common::TRACK_SEARCH_PATH       = "DustRacingTracks";

// **** Editor variables ****
QString Config::Editor::SELECT_PATH             = ":/cursor.png";
QString Config::Editor::ERASE_PATH              = ":/cross.png";
QString Config::Editor::CLEAR_PATH              = ":/clear.png";
QString Config::Editor::EDITOR_NAME             = "Dust Racing 2D Track Editor";
QString Config::Editor::EDITOR_VERSION          = VERSION;
QString Config::Editor::MODEL_CONFIG_FILE_NAME  = "editorModels.conf";
QString Config::Editor::QSETTINGS_SOFTWARE_NAME = "Editor";

// **** Game variables ****
QString Config::Game::GAME_NAME                 = "Dust Racing 2D";
QString Config::Game::GAME_VERSION              = VERSION;
QString Config::Game::QSETTINGS_SOFTWARE_NAME   = "Game";
const unsigned int Config::Game::WINDOW_WIDTH   = 1024;
const unsigned int Config::Game::WINDOW_HEIGHT  = 768;

