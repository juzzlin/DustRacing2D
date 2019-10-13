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
static const char * SETTINGS_GROUP_LAP = "LapRecords";
static const char * SETTINGS_GROUP_RACE = "RaceRecords";
static const char * SETTINGS_GROUP_POS = "BestPositions";
static const char * SETTINGS_GROUP_UNLOCK = "UnlockedTracks";

static QString combine(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty)
{
    return (QString("%1_%2_%3").arg(track.trackData().name()).arg(lapCount)).arg(static_cast<int>(difficulty));
}

static QString combineBase64(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty)
{
    return combine(track, lapCount, difficulty).toLatin1().toBase64();
}

QString Settings::difficultyKey()
{
    return "difficulty";
}

QString Settings::fpsKey()
{
    return "fps";
}

QString Settings::lapCountKey()
{
    return "lapCount";
}

QString Settings::soundsKey()
{
    return "sounds";
}

QString Settings::screenKey()
{
    return "screen";
}

QString Settings::vsyncKey()
{
    return "vsync";
}

Settings::Settings()
{
    assert(!Settings::m_instance);
    Settings::m_instance = this;

    m_actionToStringMap[InputHandler::Action::Up] = "IA_UP";
    m_actionToStringMap[InputHandler::Action::Down] = "IA_DOWN";
    m_actionToStringMap[InputHandler::Action::Left] = "IA_LEFT";
    m_actionToStringMap[InputHandler::Action::Right] = "IA_RIGHT";
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

void Settings::saveRaceRecord(const Track & track, int msecs, int lapCount, DifficultyProfile::Difficulty difficulty)
{
    QSettings settings;
    const QString trackNameAndLapCount = combine(track, lapCount, difficulty);

    settings.beginGroup(SETTINGS_GROUP_RACE);
    settings.setValue(trackNameAndLapCount, msecs);
    settings.endGroup();
}

int Settings::loadRaceRecord(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const
{
    QSettings settings;
    const QString trackNameAndLapCount = combine(track, lapCount, difficulty);

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

void Settings::saveBestPos(const Track & track, int pos, int lapCount, DifficultyProfile::Difficulty difficulty)
{
    QSettings settings;
    const QString trackNameAndLapCount = combine(track, lapCount, difficulty);

    settings.beginGroup(SETTINGS_GROUP_POS);
    settings.setValue(trackNameAndLapCount, pos);
    settings.endGroup();
}

int Settings::loadBestPos(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const
{
    QSettings settings;
    const QString trackNameAndLapCount = combine(track, lapCount, difficulty);

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

void Settings::saveTrackUnlockStatus(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty)
{
    QSettings settings;
    const QString trackNameAndLapCount = combineBase64(track, lapCount, difficulty);

    settings.beginGroup(SETTINGS_GROUP_UNLOCK);
    settings.setValue(trackNameAndLapCount, !track.trackData().isLocked());
    settings.endGroup();
}

bool Settings::loadTrackUnlockStatus(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const
{
    QSettings settings;
    const QString trackNameAndLapCount = combineBase64(track, lapCount, difficulty);

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

void Settings::saveResolution(int hRes, int vRes, bool fullScreen)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    settings.setValue("hRes", hRes);
    settings.setValue("vRes", vRes);
    settings.setValue("fullScreen", fullScreen);
    settings.endGroup();
}

void Settings::loadResolution(int & hRes, int & vRes, bool & fullScreen)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    fullScreen = settings.value("fullScreen", true).toBool();
    hRes = settings.value("hRes", 0).toInt();
    vRes = settings.value("vRes", 0).toInt();
    settings.endGroup();
}

void Settings::saveVSync(int value)
{
    saveValue(Settings::vsyncKey(), value);
}

int Settings::loadVSync()
{
    return loadValue(Settings::vsyncKey(), 1); // On by default
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

QString Settings::combineActionAndPlayer(int player, InputHandler::Action action)
{
    return QString("%2_%1").arg(player).arg(m_actionToStringMap[action]);
}

void Settings::saveKeyMapping(int player, InputHandler::Action action, int key)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    settings.setValue(combineActionAndPlayer(player, action), key);
    settings.endGroup();
}

int Settings::loadKeyMapping(int player, InputHandler::Action action)
{
    QSettings settings;

    int key = 0;
    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    key = settings.value(combineActionAndPlayer(player, action), 0).toInt();
    settings.endGroup();

    return key;
}

void Settings::saveDifficulty(DifficultyProfile::Difficulty difficulty)
{
    QSettings settings;

    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    settings.setValue(difficultyKey(), static_cast<int>(difficulty));
    settings.endGroup();
}

DifficultyProfile::Difficulty Settings::loadDifficulty() const
{
    QSettings settings;

    DifficultyProfile::Difficulty difficulty = DifficultyProfile::Difficulty::Medium;
    settings.beginGroup(SETTINGS_GROUP_CONFIG);
    difficulty = static_cast<DifficultyProfile::Difficulty>(settings.value(difficultyKey(), 0).toInt());
    settings.endGroup();

    return difficulty;
}
