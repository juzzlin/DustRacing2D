// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "eventhandler.hpp"
#include "inputhandler.hpp"
#include "statemachine.hpp"

#include <MenuManager>

EventHandler::EventHandler(InputHandler & inputHandler)
: m_inputHandler(inputHandler)
{
}

bool EventHandler::handleKeyPressEvent(QKeyEvent * event)
{
    if (StateMachine::instance().state() != StateMachine::Menu)
    {
        return handleGameKeyPressEvent(event);
    }
    else
    {
        return handleMenuKeyPressEvent(event);
    }

    return false;
}

bool EventHandler::handleKeyReleaseEvent(QKeyEvent * event)
{
    if (StateMachine::instance().state() != StateMachine::Menu)
    {
        return handleGameKeyReleaseEvent(event);
    }

    return false;
}

bool EventHandler::handleMenuKeyPressEvent(QKeyEvent * event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        MTFH::MenuManager::instance().left();
        break;
    case Qt::Key_Right:
        MTFH::MenuManager::instance().right();
        break;
    case Qt::Key_Up:
        MTFH::MenuManager::instance().up();
        break;
    case Qt::Key_Down:
        MTFH::MenuManager::instance().down();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        MTFH::MenuManager::instance().selectCurrentItem();
        break;
    case Qt::Key_Escape:
    case Qt::Key_Q:
        MTFH::MenuManager::instance().popMenu();
        if (MTFH::MenuManager::instance().done())
        {
            emit gameExited();
        }
        break;
    default:
        return false;
    }

    return true;
}

bool EventHandler::handleGameKeyPressEvent(QKeyEvent * event)
{
    if (!event->isAutoRepeat())
    {
        switch (event->key())
        {
        case Qt::Key_Left:
            m_inputHandler.setActionState(0, InputHandler::IA_LEFT, true);
            break;
        case Qt::Key_Right:
            m_inputHandler.setActionState(0, InputHandler::IA_RIGHT, true);
            break;
        case Qt::Key_Up:
            m_inputHandler.setActionState(0, InputHandler::IA_UP, true);
            break;
        case Qt::Key_Down:
        case Qt::Key_B:
            m_inputHandler.setActionState(0, InputHandler::IA_DOWN, true);
            break;
        case Qt::Key_P:
            emit pauseToggled();
            break;
        default:
            return false;
        }

        return true;
    }

    return false;
}

bool EventHandler::handleGameKeyReleaseEvent(QKeyEvent * event)
{
    if (!event->isAutoRepeat())
    {
        switch (event->key())
        {
        case Qt::Key_Left:
            m_inputHandler.setActionState(0, InputHandler::IA_LEFT, false);
            break;
        case Qt::Key_Right:
            m_inputHandler.setActionState(0, InputHandler::IA_RIGHT, false);
            break;
        case Qt::Key_Up:
            m_inputHandler.setActionState(0, InputHandler::IA_UP, false);
            break;
        case Qt::Key_Down:
        case Qt::Key_B:
            m_inputHandler.setActionState(0, InputHandler::IA_DOWN, false);
            break;
        case Qt::Key_Escape:
        case Qt::Key_Q:
            StateMachine::instance().quit();
            break;
        default:
            return false;
        }

        return true;
    }

    return false;
}
