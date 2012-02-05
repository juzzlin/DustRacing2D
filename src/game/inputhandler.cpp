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

#include "inputhandler.hpp"
#include <iostream>
#include <cassert>

InputHandler::InputHandler(MCUint maxPlayers)
  : m_playerActions(maxPlayers, std::bitset<IA_END_OF_ENUM>())
  , m_enabled(true)
{}

bool InputHandler::getActionState(MCUint playerIndex, InputAction action) const
{
    assert(playerIndex < m_playerActions.size());
    return m_enabled && m_playerActions[playerIndex][action];
}

void InputHandler::setActionState(MCUint playerIndex, InputAction action, bool state)
{
    assert(playerIndex < m_playerActions.size());
    m_playerActions[playerIndex][action] = state;
}

void InputHandler::setEnabled(bool state)
{
    m_enabled = state;
}

bool InputHandler::enabled() const
{
    return m_enabled;
}
