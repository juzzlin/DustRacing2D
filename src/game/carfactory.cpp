// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "carfactory.hpp"

#include <MCAssetManager>

std::unique_ptr<Car> CarFactory::buildCar(size_t index, size_t carCount, Game & game)
{
    const int defaultPower = 200000; // This in Watts
    const float defaultDrag = 2.5f;

    static std::map<size_t, std::string> carImageMap = {
        { 0, "carPink" },
        { 1, "carGrey" },
        { 2, "carDarkRed" },
        { 3, "carGreen" },
        { 4, "carViolet" },
        { 5, "carCyan" },
        { 6, "carBrown" },
        { 7, "carDarkGreen" },
        { 8, "carBlue" },
        { 9, "carRed" },
        { 10, "carOrange" },
        { 11, "carBlack" }
    };

    Car::Description desc;

    // Select car image
    std::string carImage("carYellow");
    if (carImageMap.count(index))
    {
        carImage = carImageMap[index];
    }

    if (index == 0 || (index == 1 && game.hasTwoHumanPlayers()))
    {
        desc.power = defaultPower;
        desc.dragQuadratic = defaultDrag;
        desc.accelerationFriction = 0.55f * Game::instance().difficultyProfile().accelerationFrictionMultiplier(true);

        return std::make_unique<Car>(desc, MCAssetManager::surfaceManager().surface(carImage), index, true);
    }
    else if (game.hasComputerPlayers())
    {
        // Introduce some variance to the power of computer players so that the
        // slowest cars have less power than the human player and the fastest
        // cars have more power than the human player.
        desc.power = defaultPower / 2 + (index + 1) * defaultPower / carCount;
        desc.accelerationFriction = (0.3f + 0.4f * float(index + 1) / carCount) * Game::instance().difficultyProfile().accelerationFrictionMultiplier(false);
        desc.dragQuadratic = defaultDrag;

        return std::make_unique<Car>(desc, MCAssetManager::surfaceManager().surface(carImage), index, false);
    }

    return nullptr;
}
