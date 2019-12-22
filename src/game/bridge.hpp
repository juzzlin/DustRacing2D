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

#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <MCObject>
#include <map>

class MCCollisionEvent;
class MCSeparationEvent;
class MCSurface;

class Bridge : public MCObject
{
public:
    Bridge();

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event) override;

    //! \reimp
    virtual void separationEvent(MCSeparationEvent & event) override;

    void objectUnderBridge(MCObject & object, bool isUnder);

    static void reset();

private:
    void doExitObject(MCObject & object);

    void raiseObject(MCObject & object, bool raise);

    using ObjectStatusMap = std::map<MCObject *, std::set<Bridge *>>;
    static ObjectStatusMap m_onBridgeStatusMap;
    static ObjectStatusMap m_underBridgeStatusMap;
};

#endif // BRIDGE_HPP
