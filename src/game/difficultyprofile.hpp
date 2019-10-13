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

#ifndef DIFFICULTYPROFILE_HPP
#define DIFFICULTYPROFILE_HPP

#include <QObject>

/** A singleton class that stores configurations
 *  for different difficulty levels. */
class DifficultyProfile : public QObject
{
    Q_OBJECT

public:
    enum class Difficulty
    {
        Easy = 0,
        Medium,
        Hard
    };

    /*! Constructor.
     *  \param difficulty The active difficulty setting. */
    DifficultyProfile(Difficulty difficulty);

    //! Destructor.
    ~DifficultyProfile();

    //! \return The singleton.
    static DifficultyProfile & instance();

    //! Set the active difficulty.
    void setDifficulty(Difficulty difficulty);

    //! \return the active difficulty.
    Difficulty difficulty() const;

    //! \return if tires are allowed to wear out.
    bool hasTireWearOut() const;

    //! \return if body damage is allowed.
    bool hasBodyDamage() const;

    float accelerationFrictionMultiplier(bool isHuman) const;

signals:

    void difficultyChanged();

private:
    DifficultyProfile(DifficultyProfile & other) = delete;

    DifficultyProfile & operator=(DifficultyProfile & other) = delete;

    static DifficultyProfile * m_instance;

    Difficulty m_difficulty;
};

#endif // DIFFICULTYPROFILE_HPP
