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

#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP

#include "inputhandler.hpp"

#include <QKeyEvent>
#include <QObject>

#include <string>

//! Handles key and mouse events.
class EventHandler : public QObject
{
    Q_OBJECT

public:

    //! Constructor.
    EventHandler(InputHandler & inputHandler);

    void enableCaptureMode(InputHandler::InputAction action, int player);

    bool handleKeyPressEvent(QKeyEvent * event);

    bool handleKeyReleaseEvent(QKeyEvent * event);

    bool handleMousePressEvent(QMouseEvent * event, int screenWidth, int screenHeight, bool mirrorY);

    bool handleMouseReleaseEvent(QMouseEvent * event, int screenWidth, int screenHeight, bool mirrorY);

signals:

    void pauseToggled();

    void gameExited();

    void soundRequested(const std::string & handle);

private:

    class ActionMapping
    {
    public:

        ActionMapping(int player, InputHandler::InputAction action)
        : m_player(player)
        , m_action(action)
        {}

        ActionMapping()
        {}

        int player() const
        {
            return m_player;
        }

        InputHandler::InputAction action() const
        {
            return m_action;
        }

    private:

        int m_player;
        InputHandler::InputAction m_action;
    };

    bool applyMatchingAction(QKeyEvent * event, bool press);

    void disableCaptureMode();

    bool handleMenuKeyPressEvent(QKeyEvent * event);

    bool handleGameKeyPressEvent(QKeyEvent * event);

    bool handleGameKeyReleaseEvent(QKeyEvent * event);

    void loadKeyMappings();

    bool mapKeyToAction(int player, InputHandler::InputAction action, int key);

    typedef std::map<int, ActionMapping> KeyToActionMap;

    KeyToActionMap            m_keyToActionMap;
    InputHandler            & m_inputHandler;
    bool                      m_captureMode;
    InputHandler::InputAction m_captureAction;
    int                       m_capturePlayer;
};

#endif // EVENTHANDLER_HPP
