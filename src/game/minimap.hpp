// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MINIMAP_HPP
#define MINIMAP_HPP

#include <map>
#include <vector>

#include "car.hpp"

#include <MCVector3d>

class MapBase;
class MCSurface;
class Race;
class TrackTile;

class Minimap
{
public:
    Minimap();

    Minimap(Car & carToFollow, const MapBase & trackMap, int x, int y, int size);

    void initialize(Car & carToFollow, const MapBase & trackMap, int x, int y, int size);

    using CarVector = std::vector<CarPtr>;
    void render(const CarVector & cars, const Race & race);

private:
    void renderMap();

    void renderMarkers(const CarVector & cars, const Race & race);

    struct MinimapTile
    {
        MCVector3dF pos;

        int rotation;
    };

    std::map<MCSurface *, std::vector<MinimapTile>> m_map;

    Car * m_carToFollow = nullptr;

    MCSurface * m_markerSurface = nullptr;

    MCVector3dF m_center;

    MCVector3dF m_size;

    float m_tileW = 0;

    float m_tileH = 0;

    float m_sceneW = 0;

    float m_sceneH = 0;
};

#endif // MINIMAP_HPP
