// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "settings.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "../common/config.hpp"
#include <QSettings>
#include <cassert>

Settings * Settings::m_instance = nullptr;

static const char * SETTINGS_GROUP_LAP = "LapRecords";
static const char * SETTINGS_GROUP_POS = "BestPositions";

Settings::Settings()
{
    assert(!Settings::m_instance);
    Settings::m_instance = this;
}

Settings & Settings::instance()
{
    assert(Settings::m_instance);
    return *Settings::m_instance;
}

void Settings::saveLapRecord(Track & track, int msecs)
{
    // Open settings file
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_LAP);
    settings.setValue(track.trackData().name(), msecs);
    settings.endGroup();
}

int Settings::loadLapRecord(Track & track) const
{
    // Open settings file
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    // Read record time, -1 if not found
    settings.beginGroup(SETTINGS_GROUP_LAP);
    const int time = settings.value(track.trackData().name(), -1).toInt();
    settings.endGroup();

    return time;
}

void Settings::saveBestPos(Track & track, int pos)
{
    // Open settings file
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_POS);
    settings.setValue(track.trackData().name(), pos);
    settings.endGroup();
}

int Settings::loadBestPos(Track & track) const
{
    // Open settings file
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    // Read the best position, -1 if not found
    settings.beginGroup(SETTINGS_GROUP_POS);
    const int pos = settings.value(track.trackData().name(), -1).toInt();
    settings.endGroup();

    return pos;
}
