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

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "difficultyprofile.hpp"

#include <map>
#include <mutex>

class Track;

class Database
{
public:
    Database();

    void saveLapRecord(const Track & track, int msecs);

    std::pair<int, bool> loadLapRecord(const Track & track) const;

    void resetLapRecords();

    void saveRaceRecord(const Track & track, int msecs, int lapCount, DifficultyProfile::Difficulty difficulty);

    std::pair<int, bool> loadRaceRecord(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const;

    void resetRaceRecords();

    void saveBestPos(const Track & track, int pos, int lapCount, DifficultyProfile::Difficulty difficulty);

    std::pair<int, bool> loadBestPos(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const;

    void resetBestPos();

    void saveTrackUnlockStatus(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty);

    bool loadTrackUnlockStatus(const Track & track, int lapCount, DifficultyProfile::Difficulty difficulty) const;

    void resetTrackUnlockStatuses();

    static Database & instance();

private:
    void initialize();

    static Database * m_instance;

    mutable std::mutex m_mutex;
};

#endif // DATABASE_HPP
