// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "settings.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include <QSettings>
#include <cassert>

Settings * Settings::m_instance = nullptr;

static const char * SETTINGS_GROUP_CONFIG = "Config";
static const char * SETTINGS_GROUP_LAP    = "LapRecords";
static const char * SETTINGS_GROUP_RACE   = "RaceRecords";
static const char * SETTINGS_GROUP_POS    = "BestPositions";
static const char * SETTINGS_GROUP_UNLOCK = "UnlockedTracks";

static QString combineTrackAndLapCount(const Track & track, int lapCount)
{
    return (QString("%1_%2").arg(track.trackData().name()).arg(lapCount));
}

static QString combineTrackAndLapCountBase64(const Track & track, int lapCount)
{
    return (QString("%1_%2").arg(track.trackData().name()).arg(lapCount)).toLatin1().toBase64();
}

QString Settings::lapCountKey()
{
    return "lapCount";
}
QString Settings::soundsKey()
{
    return "sounds";
}

QString Settings::vsyncKey()
{
    return "vsync";
}

Settings::Settings()
{
    assert(!Settings::m_instance);
    Settings::m_instance = this;

    m_actionToStringMap[InputHandler::IA_UP]    = "IA_UP";
    m_actionToStringMap[InputHandler::IA_DOWN]  = "IA_DOWN";
    m_actionToStringMap[InputHandler::IA_LEFT]  = "IA_LEFT";
    m_actionToStringMap[InputHandler::IA_RIGHT] = "IA_RIGHT";
}

Settings & Settings::instance()
{
    assert(Settings::m_instance);
    return *Settings::m_instance;
}

void Settings::saveLapRecord(const Track & track, int msecs)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_LAP);
    settings.setValue(track.trackData().name(), msecs);
    settings.endGroup();
}

int Settings::loadLapRecord(const Track & track) const
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_LAP);
    const int time = settings.value(track.trackData().name(), -1).toInt();
    settings.endGroup();

    return time;
}

void Settings::resetLapRecords()
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_LAP);
    settings.remove("");
    settings.endGroup();
}

void Settings::saveRaceRecord(const Track & track, int msecs, int lapCount)
{
    QSettings settings;
    const QString trackNameAndLapCount = combineTrackAndLapCount(track, lapCount);

    settings.beginGroup(SETTINGS_GROUP_RACE);
    settings.setValue(trackNameAndLapCount, msecs);
    settings.endGroup();
}

int Settings::loadRaceRecord(const Track & track, int lapCount) const
{
    QSettings settings;
    const QString trackNameAndLapCount = combineTrackAndLapCount(track, lapCount);

    settings.beginGroup(SETTINGS_GROUP_RACE);
    const int time = settings.value(trackNameAndLapCount, -1).toInt();
    settings.endGroup();

    return time;
}

void Settings::resetRaceRecords()
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_RACE);
    settings.remove("");
    settings.endGroup();
}

void Settings::saveBestPos(const Track & track, int pos, int lapCount)
{
    QSettings settings;
    const QString trackNameAndLapCount = combineTrackAndLapCount(track, lapCount);

    settings.beginGroup(SETTINGS_GROUP_POS);
    settings.setValue(trackNameAndLapCount, pos);
    settings.endGroup();
}

int Settings::loadBestPos(const Track & track, int lapCount) const
{
    QSettings settings;
    const QString trackNameAndLapCount = combineTrackAndLapCount(track, lapCount);

    settings.beginGroup(SETTINGS_GROUP_POS);
    const int pos = settings.value(trackNameAndLapCount, -1).toInt();
    settings.endGroup();

    return pos;
}

void Settings::resetBestPos()
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_POS);
    settings.remove("");
    settings.endGroup();
}

void Settings::saveTrackUnlockStatus(const Track & track, int lapCount)
{
    QSettings settings;
    const QString trackNameAndLapCount = combineTrackAndLapCountBase64(track, lapCount);

    settings.beginGroup(SETTINGS_GROUP_UNLOCK);
    settings.setValue(trackNameAndLapCount, !track.trackData().isLocked());
    settings.endGroup();
}

bool Settings::loadTrackUnlockStatus(const Track & track, int lapCount) const
{
    QSettings settings;
    const QString trackNameAndLapCount = combineTrackAndLapCountBase64(track, lapCount);

    settings.beginGroup(SETTINGS_GROUP_UNLOCK);
    const bool status = settings.value(trackNameAndLapCount, 0).toBool();
    settings.endGroup();

    return status;
}

void Settings::resetTrackUnlockStatuses()
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_UNLOCK);
    settings.remove("");
    settings.endGroup();
}

void Settings::saveResolution(int hRes, int vRes, bool nativeResolution, bool fullScreen)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    settings.setValue("hRes", hRes);
    settings.setValue("vRes", vRes);
    settings.setValue("nativeResolution", nativeResolution);
    settings.setValue("fullScreen", fullScreen);
    settings.endGroup();
}

void Settings::loadResolution(int & hRes, int & vRes, bool & nativeResolution, bool & fullScreen)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    nativeResolution = settings.value("nativeResolution", true).toBool();
    fullScreen       = settings.value("fullScreen", true).toBool();
    hRes             = settings.value("hRes", 0).toInt();
    vRes             = settings.value("vRes", 0).toInt();
    settings.endGroup();
}

void Settings::saveValue(QString key, int value)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    settings.setValue(key, value);
    settings.endGroup();
}

int Settings::loadValue(QString key, int defaultValue)
{
    QSettings settings;

    int value = 0;
    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    value = settings.value(key, defaultValue).toInt();
    settings.endGroup();

    return value;
}

QString Settings::combineActionAndPlayer(int player, InputHandler::InputAction action)
{
    return QString("%2_%1").arg(player).arg(m_actionToStringMap[action]);
}

void Settings::saveKeyMapping(int player, InputHandler::InputAction action, int key)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    settings.setValue(combineActionAndPlayer(player, action), key);
    settings.endGroup();
}

int Settings::loadKeyMapping(int player, InputHandler::InputAction action)
{
    QSettings settings;

    int key = 0;
    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    key = settings.value(combineActionAndPlayer(player, action), 0).toInt();
    settings.endGroup();

    return key;
}
