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

#include <QDir>
#include <QProcessEnvironment>

//! Config variables for editor and for the game.
namespace Config {

namespace General {
//! The base data path given by -DDATA_PATH.
static constexpr auto dataPath = DATA_PATH;

static constexpr auto COPYRIGHT = "Copyright (c) 2011-2019 Jussi Lind";

//! Organization name given to Qt. Affects QSettings and SQLite database file locations.
static constexpr auto QT_ORGANIZATION_NAME = "DustRacing2D";

//! Path used to search "3rd party" race tracks under the home dir: ~/TRACK_SEARCH_PATH/
static constexpr auto TRACK_SEARCH_PATH = "DustRacingTracks";

static constexpr auto WEB_SITE_URL = "http://juzzlin.github.io/DustRacing2D";

namespace Unix {
//! Path used to search "3rd party" race tracks under `$XDG_DATA_HOME/TRACK_SEARCH_PATH_XDG` or
//! $HOME/.local/share/TRACK_SEARCH_PATH_XDG` if `$XDG_DATA_HOME` is not defined.
//! TODO: Make this use QStandardPaths like in database.cpp !
static constexpr auto TRACK_SEARCH_PATH_XDG = "DustRacing2D/tracks";
inline QString getXdgTrackSearchPath()
{
    const auto env = QProcessEnvironment::systemEnvironment();
    const auto xdgDataHome = "XDG_DATA_HOME";
    return env.value(xdgDataHome, QDir::homePath() + "/.local/share") + "/" + TRACK_SEARCH_PATH_XDG;
}
} // namespace Unix

} // namespace General

namespace Editor {
static constexpr auto EDITOR_NAME = "Dust Racing 2D Track Editor";

static constexpr auto EDITOR_VERSION = VERSION;

static constexpr auto MODEL_CONFIG_FILE_NAME = "editorModels.conf";

static constexpr auto QT_APPLICATION_NAME = "Editor";

static constexpr auto SELECT_ICON_PATH = ":/cursor.png";

static constexpr auto ERASE_ICON_PATH = ":/cross.png";

static constexpr auto CLEAR_ICON_PATH = ":/clear.png";
} // namespace Editor

namespace Game {
static constexpr auto GAME_NAME = "Dust Racing 2D";

static constexpr auto GAME_VERSION = VERSION;

static constexpr auto QT_APPLICATION_NAME = "Game";

static constexpr auto SQLITE_DATABASE_FILE_NAME = "status.db";
} // namespace Game

} // namespace Config
