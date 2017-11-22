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

#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <vector>
#include <bitset>



//! Handler for players' interactions.
class InputHandler
{
public:

    //! Possible actions.
    enum class Action : int
    {
        Left = 0,
        Right,
        Up,
        Down,
        EndOfEnum
    };

    //! Constructor.
    InputHandler(unsigned int maxPlayers);

    //! Set state of the given action of the given player.
    void setActionState(unsigned int playerIndex, Action action, bool state);

    //! Get state of the given action of the given player.
    bool getActionState(unsigned int playerIndex, Action action) const;

    //! Reset the current actions.
    void reset();

    //! Enable/disable controls. Enabled by default.
    static void setEnabled(bool state);

    //! Return true if enabled.
    static bool enabled();

private:

    typedef std::vector<std::bitset<static_cast<int>(Action::EndOfEnum)>> ActionVector;

    ActionVector m_playerActions;

    static bool m_enabled;
};

#endif // INPUTHANDLER_HPP
