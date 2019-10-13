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

#include "difficultyprofile.hpp"

#include <cassert>

DifficultyProfile * DifficultyProfile::m_instance = nullptr;

DifficultyProfile::DifficultyProfile(Difficulty difficulty)
  : m_difficulty(difficulty)
{
    assert(!m_instance);
    m_instance = this;
}

DifficultyProfile & DifficultyProfile::instance()
{
    assert(m_instance);
    return *m_instance;
}

void DifficultyProfile::setDifficulty(Difficulty difficulty)
{
    m_difficulty = difficulty;

    emit difficultyChanged();
}

DifficultyProfile::Difficulty DifficultyProfile::difficulty() const
{
    return m_difficulty;
}

bool DifficultyProfile::hasTireWearOut() const
{
    return true;
}

bool DifficultyProfile::hasBodyDamage() const
{
    return true;
}

float DifficultyProfile::accelerationFrictionMultiplier(bool isHuman) const
{
    switch (m_difficulty)
    {
    case Difficulty::Easy:
        return isHuman ? 0.70f : 0.70f * 0.85f;
    case Difficulty::Medium:
        return isHuman ? 0.85f : 0.85f * 0.85f;
    case Difficulty::Hard:
        return isHuman ? 1.0f : 1.0f * 0.9f;
    }

    return 1.0f;
}

DifficultyProfile::~DifficultyProfile()
{
    m_instance = nullptr;
}
