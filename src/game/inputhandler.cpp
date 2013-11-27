// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "inputhandler.hpp"
#include <iostream>
#include <cassert>

bool InputHandler::m_enabled = true;

InputHandler::InputHandler(MCUint maxPlayers)
  : m_playerActions(maxPlayers, std::bitset<IA_END_OF_ENUM>())
{}

bool InputHandler::getActionState(MCUint playerIndex, InputAction action) const
{
    return InputHandler::m_enabled && m_playerActions[playerIndex][action];
}

void InputHandler::setActionState(MCUint playerIndex, InputAction action, bool state)
{
    m_playerActions[playerIndex][action] = state;
}

void InputHandler::setEnabled(bool state)
{
    m_enabled = state;
}

bool InputHandler::enabled()
{
    return m_enabled;
}

void InputHandler::reset()
{
    for (auto & i : m_playerActions)
    {
        i.reset();
    }
}
