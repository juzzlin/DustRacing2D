// This file is part of Dust Racing 2D.
// Copyright (C) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#include "database.hpp"
#include "../common/config.hpp"
#include "../contrib/SimpleLogger/src/simple_logger.hpp"
#include "track.hpp"
#include "trackdata.hpp"

#include <stdexcept>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QVariant>

using juzzlin::L;

namespace {

const QString BEST_POSITION_TABLE = "best_position";

const QString BEST_POSITION_FILTER = "track_name = :track_name AND version = :version AND lap_count = :lap_count AND difficulty = :difficulty";

const QString LAP_RECORD_TABLE = "lap_record";

const QString LAP_RECORD_FILTER = "track_name = :track_name AND version = :version";

const QString RACE_RECORD_TABLE = "race_record";

const QString RACE_RECORD_FILTER = "track_name = :track_name AND version = :version AND lap_count = :lap_count AND difficulty = :difficulty";

const QString TRACK_UNLOCK_TABLE = "track_unlock";

const QString TRACK_UNLOCK_FILTER = "track_name = :track_name AND version = :version AND lap_count = :lap_count AND difficulty = :difficulty";

const int TRACK_SET_VERSION = 1;

QString getAppDataPath()
{
    const auto dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (const QFileInfo path(dataPath); !path.exists())
    {
        QDir().mkpath(dataPath);
    }
    return dataPath;
}

QString getDbFilePath()
{
    return getAppDataPath() + QDir::separator() + Config::Game::SQLITE_DATABASE_FILE_NAME;
}

void printError(QSqlQuery & query)
{
    L().error() << "SQL Error: '" << query.lastQuery().toStdString() << "': '" << query.lastError().text().toStdString() << "'";
}
} // namespace

Database * Database::m_instance = nullptr;

Database::Database()
{
    if (!Database::m_instance)
    {
        Database::m_instance = this;
    }
    else
    {
        throw std::runtime_error("Database already instantiated!");
    }

    initialize();
}

void Database::initialize()
{
    L().info() << "Creating SQLite database file at " << getDbFilePath().toStdString();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(getDbFilePath());
    db.open();

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS " + BEST_POSITION_TABLE + " (track_name TEXT, version INTEGER, lap_count INTEGER, difficulty INTEGER, position INTEGER)"))
    {
        printError(query);
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS " + LAP_RECORD_TABLE + " (track_name TEXT, version INTEGER, time INTEGER)"))
    {
        printError(query);
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS " + RACE_RECORD_TABLE + " (track_name TEXT, version INTEGER, lap_count INTEGER, difficulty INTEGER, time INTEGER)"))
    {
        printError(query);
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS " + TRACK_UNLOCK_TABLE + " (track_name TEXT, version INTEGER, lap_count INTEGER, difficulty INTEGER)"))
    {
        printError(query);
    }
}

Database & Database::instance()
{
    return *m_instance;
}

void Database::saveLapRecord(const Track & track, int msecs)
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    query.prepare("SELECT time FROM " + LAP_RECORD_TABLE + " WHERE " + LAP_RECORD_FILTER);
    query.bindValue(":version", TRACK_SET_VERSION);
    query.bindValue(":track_name", track.trackData().name());
    if (!query.exec())
    {
        printError(query);
        return;
    }

    if (!query.first())
    {
        L().debug() << "New lap record database entry added for " << track.trackData().name().toStdString();

        QSqlQuery query;
        query.prepare("INSERT INTO " + LAP_RECORD_TABLE + " (track_name, version, time) VALUES (:track_name, :version, :time)");
        query.bindValue(":version", TRACK_SET_VERSION);
        query.bindValue(":track_name", track.trackData().name());
        query.bindValue(":time", msecs);
        if (!query.exec())
        {
            printError(query);
        }
    }
    else
    {
        L().debug() << "Updated lap record database entry for " << track.trackData().name().toStdString();

        QSqlQuery query;
        query.prepare("UPDATE " + LAP_RECORD_TABLE + " SET track_name = :track_name, version = :version, time = :time WHERE " + LAP_RECORD_FILTER);
        query.bindValue(":version", TRACK_SET_VERSION);
        query.bindValue(":track_name", track.trackData().name());
        query.bindValue(":time", msecs);
        if (!query.exec())
        {
            printError(query);
        }
    }
}

std::pair<int, bool> Database::loadLapRecord(const Track & track) const
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    query.prepare("SELECT time FROM " + LAP_RECORD_TABLE + " WHERE " + LAP_RECORD_FILTER);
    query.bindValue(":version", TRACK_SET_VERSION);
    query.bindValue(":track_name", track.trackData().name());
    if (!query.exec())
    {
        printError(query);
        return {};
    }

    if (!query.first())
    {
        return {};
    }

    const auto record = query.record();
    const auto timeCol = record.indexOf("time");
    return { query.value(timeCol).toInt(), true };
}

void Database::resetLapRecords()
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    if (!query.exec("DELETE FROM " + LAP_RECORD_TABLE))
    {
        printError(query);
    }
}

void Database::saveRaceRecord(const Track & track, int msecs, int lapCount, DifficultyProfile::Difficulty difficulty)
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    query.prepare("SELECT time FROM " + RACE_RECORD_TABLE + " WHERE " + RACE_RECORD_FILTER);
    query.bindValue(":version", TRACK_SET_VERSION);
    query.bindValue(":track_name", track.trackData().name());
    query.bindValue(":lap_count", lapCount);
    query.bindValue(":difficulty", static_cast<int>(difficulty));
    if (!query.exec())
    {
        printError(query);
        return;
    }

    if (!query.first())
    {
        L().debug() << "New race record database entry added for " << track.trackData().name().toStdString();

        QSqlQuery query;
        query.prepare("INSERT INTO " + RACE_RECORD_TABLE + " (track_name, version, lap_count, difficulty, time) VALUES (:track_name, :version, :lap_count, :difficulty, :time)");
        query.bindValue(":version", TRACK_SET_VERSION);
        query.bindValue(":track_name", track.trackData().name());
        query.bindValue(":lap_count", lapCount);
        query.bindValue(":difficulty", static_cast<int>(difficulty));
        query.bindValue(":time", msecs);
        if (!query.exec())
        {
            printError(query);
        }
    }
    else
    {
        L().debug() << "Updated race record database entry for " << track.trackData().name().toStdString();

        QSqlQuery query;
        query.prepare("UPDATE " + RACE_RECORD_TABLE + " SET track_name = :track_name, version = :version, time = :time WHERE " + RACE_RECORD_FILTER);
        query.bindValue(":version", TRACK_SET_VERSION);
        query.bindValue(":track_name", track.trackData().name());
        query.bindValue(":lap_count", lapCount);
        query.bindValue(":difficulty", static_cast<int>(difficulty));
        query.bindValue(":time", msecs);
        if (!query.exec())
        {
            printError(query);
        }
    }
}

std::pair<int, bool> Database::loadRaceRecord(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    query.prepare("SELECT time FROM " + RACE_RECORD_TABLE + " WHERE " + RACE_RECORD_FILTER);
    query.bindValue(":version", TRACK_SET_VERSION);
    query.bindValue(":track_name", track.trackData().name());
    query.bindValue(":lap_count", lapCount);
    query.bindValue(":difficulty", static_cast<int>(difficulty));
    if (!query.exec())
    {
        printError(query);
        return {};
    }

    if (!query.first())
    {
        return {};
    }

    const auto record = query.record();
    const auto timeCol = record.indexOf("time");
    return { query.value(timeCol).toInt(), true };
}

void Database::resetRaceRecords()
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    if (!query.exec("DELETE FROM " + RACE_RECORD_TABLE))
    {
        printError(query);
    }
}

void Database::saveBestPos(const Track & track, int pos, int lapCount, DifficultyProfile::Difficulty difficulty)
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    query.prepare("SELECT position FROM " + BEST_POSITION_TABLE + " WHERE " + BEST_POSITION_FILTER);
    query.bindValue(":version", TRACK_SET_VERSION);
    query.bindValue(":track_name", track.trackData().name());
    query.bindValue(":lap_count", lapCount);
    query.bindValue(":difficulty", static_cast<int>(difficulty));
    if (!query.exec())
    {
        printError(query);
        return;
    }

    if (!query.first())
    {
        L().debug() << "New best position database entry added for " << track.trackData().name().toStdString();

        QSqlQuery query;
        query.prepare("INSERT INTO " + BEST_POSITION_TABLE + " (track_name, version, lap_count, difficulty, position) VALUES (:track_name, :version, :lap_count, :difficulty, :position)");
        query.bindValue(":version", TRACK_SET_VERSION);
        query.bindValue(":track_name", track.trackData().name());
        query.bindValue(":lap_count", lapCount);
        query.bindValue(":difficulty", static_cast<int>(difficulty));
        query.bindValue(":position", pos);
        if (!query.exec())
        {
            printError(query);
        }
    }
    else
    {
        L().debug() << "Updated best position database entry for " << track.trackData().name().toStdString();

        QSqlQuery query;
        query.prepare("UPDATE " + BEST_POSITION_TABLE + " SET track_name = :track_name, version = :version, position = :position WHERE " + BEST_POSITION_FILTER);
        query.bindValue(":version", TRACK_SET_VERSION);
        query.bindValue(":track_name", track.trackData().name());
        query.bindValue(":lap_count", lapCount);
        query.bindValue(":difficulty", static_cast<int>(difficulty));
        query.bindValue(":position", pos);
        if (!query.exec())
        {
            printError(query);
        }
    }
}

std::pair<int, bool> Database::loadBestPos(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    query.prepare("SELECT position FROM " + BEST_POSITION_TABLE + " WHERE " + BEST_POSITION_FILTER);
    query.bindValue(":version", TRACK_SET_VERSION);
    query.bindValue(":track_name", track.trackData().name());
    query.bindValue(":lap_count", lapCount);
    query.bindValue(":difficulty", static_cast<int>(difficulty));
    if (!query.exec())
    {
        printError(query);
        return {};
    }

    if (!query.first())
    {
        return {};
    }

    const auto record = query.record();
    const auto timeCol = record.indexOf("position");
    return { query.value(timeCol).toInt(), true };
}

void Database::resetBestPos()
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    if (!query.exec("DELETE FROM " + BEST_POSITION_TABLE))
    {
        printError(query);
    }
}

void Database::saveTrackUnlockStatus(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty)
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    query.prepare("SELECT * FROM " + TRACK_UNLOCK_TABLE + " WHERE " + TRACK_UNLOCK_FILTER);
    query.bindValue(":version", TRACK_SET_VERSION);
    query.bindValue(":track_name", track.trackData().name());
    query.bindValue(":lap_count", lapCount);
    query.bindValue(":difficulty", static_cast<int>(difficulty));
    if (!query.exec())
    {
        printError(query);
        return;
    }

    if (!query.first())
    {
        L().debug() << "New track unlock database entry added for " << track.trackData().name().toStdString();

        QSqlQuery query;
        query.prepare("INSERT INTO " + TRACK_UNLOCK_TABLE + " (track_name, version, lap_count, difficulty) VALUES (:track_name, :version, :lap_count, :difficulty)");
        query.bindValue(":version", TRACK_SET_VERSION);
        query.bindValue(":track_name", track.trackData().name());
        query.bindValue(":lap_count", lapCount);
        query.bindValue(":difficulty", static_cast<int>(difficulty));
        if (!query.exec())
        {
            printError(query);
        }
    }
}

bool Database::loadTrackUnlockStatus(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    query.prepare("SELECT * FROM " + TRACK_UNLOCK_TABLE + " WHERE " + TRACK_UNLOCK_FILTER);
    query.bindValue(":version", TRACK_SET_VERSION);
    query.bindValue(":track_name", track.trackData().name());
    query.bindValue(":lap_count", lapCount);
    query.bindValue(":difficulty", static_cast<int>(difficulty));
    if (!query.exec())
    {
        printError(query);
        return false;
    }

    return query.first();
}

void Database::resetTrackUnlockStatuses()
{
    std::lock_guard<std::mutex> lock { m_mutex };

    QSqlQuery query;
    if (!query.exec("DELETE FROM " + TRACK_UNLOCK_TABLE))
    {
        printError(query);
    }
}
