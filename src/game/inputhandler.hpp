// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <vector>
#include <bitset>

#include "MiniCore/Core/MCTypes"

//! Handler for players' interactions.
class InputHandler
{
public:

    //! Possible actions.
    enum InputAction {IA_LEFT = 0, IA_RIGHT, IA_UP, IA_DOWN, IA_END_OF_ENUM};

    //! Constructor.
    InputHandler(MCUint maxPlayers);

    //! Set state of the given action of the given player.
    void setActionState(MCUint playerIndex, InputAction action, bool state);

    //! Get state of the given action of the given player.
    bool getActionState(MCUint playerIndex,
        InputAction action) const;

    //! Enable/disable controls. Enabled by default.
    void setEnabled(bool state);

    bool enabled() const;

private:

    typedef std::vector<std::bitset<IA_END_OF_ENUM> > ActionVector;
    ActionVector m_playerActions;
    bool m_enabled;
};

#endif // INPUTHANDLER_HPP
