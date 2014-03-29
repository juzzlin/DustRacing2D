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

CarPtr CarFactory::buildCar(int index, int numCars, Game & game)
{
    const int   defaultPower = 200000; // This in Watts
    const float defaultDrag  = 5.0;

    static const int NUM_CARS = numCars;
    static std::map<int, std::string> carImageMap = {
        {NUM_CARS - 1, "carBlack"    },
        {NUM_CARS - 2, "carOrange"   },
        {NUM_CARS - 3, "carRed"      },
        {NUM_CARS - 4, "carBlue"     },
        {NUM_CARS - 5, "carDarkGreen"},
        {NUM_CARS - 6, "carBrown"    },
        {1,            "carGrey"     },
        {0,            "carPink"     }
    };

    Car::Description desc;

    // Select car image
    std::string carImage("carYellow");
    if (carImageMap.count(index))
    {
        carImage = carImageMap[index];
    }

    CarPtr car;
    if (index == 0 || (index == 1 && game.hasTwoHumanPlayers()))
    {
        desc.power                = 0.75 * defaultPower;
        desc.dragQuadratic        = defaultDrag;
        desc.accelerationFriction = 0.55;

        car.reset(new Car(desc, MCAssetManager::surfaceManager().surface(carImage), index, true));
    }
    else if (game.hasComputerPlayers())
    {
        // Introduce some variance to the power of computer players so that the
        // slowest cars have less power than the human player and the fastest
        // cars have more power than the human player.
        desc.power                = defaultPower / 2 + (index + 1) * defaultPower / NUM_CARS;
        desc.accelerationFriction = 0.3 + 0.4 * float(index + 1) / NUM_CARS;
        desc.dragQuadratic        = defaultDrag;
        desc.brakingFriction      = 2.0;

        car.reset(new Car(desc, MCAssetManager::surfaceManager().surface(carImage), index, false));
    }

    return car;
}
