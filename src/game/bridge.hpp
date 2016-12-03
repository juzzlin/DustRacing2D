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
class MCSurface;
class Car;

class Bridge : public MCObject
{
public:

    static MCFloat zOffset()
    {
        return 5.0f;
    }

    Bridge(MCSurface & surface, MCSurface & rail);

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event) override;

    //! \reimp
    virtual void onStepTime(int step) override;

    void enterObject(MCObject & object);

private:

    std::map<MCObject *, int> m_objectsOnBridge;

    std::map<MCObject *, bool> m_objectsEntered;

    int m_tag;

    MCObjectPtr m_rail0;
    MCObjectPtr m_rail1;
    MCObjectPtr m_trigger0;
    MCObjectPtr m_trigger1;
};

#endif // BRIDGE_HPP
