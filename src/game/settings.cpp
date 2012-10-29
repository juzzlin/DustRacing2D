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

static const char * SETTINGS_GROUP_CONFIG = "Config";
static const char * SETTINGS_GROUP_LAP    = "LapRecords";
static const char * SETTINGS_GROUP_POS    = "BestPositions";
static const char * SETTINGS_GROUP_UNLOCK = "UnlockedTracks";

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

void Settings::saveLapRecord(const Track & track, int msecs)
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_LAP);
    settings.setValue(track.trackData().name(), msecs);
    settings.endGroup();
}

int Settings::loadLapRecord(const Track & track) const
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_LAP);
    const int time = settings.value(track.trackData().name(), -1).toInt();
    settings.endGroup();

    return time;
}

void Settings::resetLapRecords()
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_LAP);
    settings.remove("");
    settings.endGroup();
}

void Settings::saveBestPos(const Track & track, int pos)
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_POS);
    settings.setValue(track.trackData().name(), pos);
    settings.endGroup();
}

int Settings::loadBestPos(const Track & track) const
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_POS);
    const int pos = settings.value(track.trackData().name(), -1).toInt();
    settings.endGroup();

    return pos;
}

void Settings::resetBestPos()
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_POS);
    settings.remove("");
    settings.endGroup();
}

void Settings::saveTrackUnlockStatus(const Track & track)
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_UNLOCK);
    settings.setValue(track.trackData().name().toLatin1().toBase64(),
        !track.trackData().isLocked());
    settings.endGroup();
}

bool Settings::loadTrackUnlockStatus(const Track & track) const
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_UNLOCK);
    const bool status = settings.value(
        track.trackData().name().toLatin1().toBase64(), 0).toBool();
    settings.endGroup();

    return status;
}

void Settings::resetTrackUnlockStatuses()
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_UNLOCK);
    settings.remove("");
    settings.endGroup();
}

void Settings::saveResolution(int hRes, int vRes, bool fullScreen)
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    settings.setValue("hRes", hRes);
    settings.setValue("vRes", vRes);
    settings.setValue("fullScreen", fullScreen);
    settings.endGroup();
}

void Settings::loadResolution(int & hRes, int & vRes, bool & fullScreen)
{
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    fullScreen = settings.value("fullScreen", false).toBool();
    hRes       = settings.value("hRes", Config::Game::WINDOW_WIDTH).toInt();
    vRes       = settings.value("vRes", Config::Game::WINDOW_HEIGHT).toInt();
    settings.endGroup();
}
