// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef GRANDSTAND_HPP
#define GRANDSTAND_HPP

#include <MCObject>
#include <MCVector2d>

#include <vector>

//! A grandstand with an animated crowd that cheers at passing cars and does the wave every now and then.
class Grandstand : public MCObject
{
public:
    Grandstand();

    //! Animates the crowd.
    void onStepTime(int step) override;

    //! Tells the locations of the cars, so that the crowd can cheer when cars pass by.
    static void setCarLocations(std::vector<MCVector2dF> locations);

private:
    struct Section
    {
        MCObjectPtr object;

        //! 0.0 is calm and 1.0 is cheering.
        float excitement = 0;

        size_t frame = 0;

        //! Time until the next change of the pose of an idle or excited crowd.
        float nextPoseChange = 0;

        //! Alternates between the two frames of the current mood.
        bool alternate = false;
    };

    void setFrame(Section & section, size_t frame);

    void updateExcitement(Section & section, float timeStep);

    std::vector<Section> m_sections;

    //! Position of the wave in sections, or negative if there's no wave going on.
    float m_wavePosition = -1;

    float m_timeToNextWave;

    static std::vector<MCVector2dF> m_carLocations;
};

#endif // GRANDSTAND_HPP
