// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

namespace Config {

const QString Common::dataPath                = DATA_PATH;

const QString Common::COPYRIGHT               = "Copyright (c) 2011-2017 Jussi Lind";
const QString Common::QSETTINGS_COMPANY_NAME  = "dustrac";
const QString Common::TRACK_SEARCH_PATH       = "DustRacingTracks";
const QString Common::WEB_SITE_URL            = "http://juzzlin.github.io/DustRacing2D";

const QString Editor::SELECT_ICON_PATH        = ":/cursor.png";
const QString Editor::ERASE_ICON_PATH         = ":/cross.png";
const QString Editor::CLEAR_ICON_PATH         = ":/clear.png";
const QString Editor::EDITOR_NAME             = "Dust Racing 2D Track Editor";
const QString Editor::EDITOR_VERSION          = VERSION;
const QString Editor::MODEL_CONFIG_FILE_NAME  = "editorModels.conf";
const QString Editor::QSETTINGS_SOFTWARE_NAME = "Editor";

const QString Game::GAME_NAME                 = "Dust Racing 2D";
const QString Game::GAME_VERSION              = VERSION;
const QString Game::QSETTINGS_SOFTWARE_NAME   = "Game";

} // Config
