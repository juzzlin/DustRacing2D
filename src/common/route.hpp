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

#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "targetnodebase.hpp"

#include <vector>

class TrackTileBase;
class TargetNodeBase;

//! Route is used to define the race route as a sequence
//! of TrackTiles.
class Route
{
public:
    //! Constructor.
    Route();

    Route(const Route & other) = delete;

    Route & operator=(const Route & other) = delete;

    typedef std::vector<TargetNodeBasePtr> RouteVector;

    //! Clear the current route.
    void clear();

    //! Push new target to the route and return true
    //! if the route got closed.
    bool push(TargetNodeBasePtr target);

    //! Build route from an (unordered) vector of Targets.
    //! Will be sorted with respect to their indices.
    void buildFromVector(RouteVector & routeVector);

    //! Return number of target nodes.
    size_t numNodes() const;

    //! Return Target for the given index.
    TargetNodeBasePtr get(size_t index) const;

    //! Get all nodes.
    void getAll(RouteVector & routeVector) const;

    //! Return length based on target node locations.
    double geometricLength() const;

    RouteVector::iterator begin();

    RouteVector::iterator end();

    RouteVector::const_iterator cbegin() const;

    RouteVector::const_iterator cend() const;

private:
    bool isClosed() const;

    std::vector<TargetNodeBasePtr> m_route;
};

#endif // ROUTE_HPP
