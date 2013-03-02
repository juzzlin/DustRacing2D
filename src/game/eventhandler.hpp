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

namespace SFX {
class Sound;
}

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

    void setMenuClickSound(SFX::Sound & menuClick);

    void setMenuSelectSound(SFX::Sound & menuSelect);

signals:

    void pauseToggled();

    void gameExited();

private:

    struct ActionMapping
    {
        int player;
        InputHandler::InputAction action;
    };

    bool applyMatchingAction(QKeyEvent * event, bool press);

    void disableCaptureMode();

    bool handleMenuKeyPressEvent(QKeyEvent * event);

    bool handleGameKeyPressEvent(QKeyEvent * event);

    bool handleGameKeyReleaseEvent(QKeyEvent * event);

    void loadKeyMappings();

    bool mapKeyToAction(int player, InputHandler::InputAction action, int key);

    void playMenuClickSound();

    void playMenuSelectSound();

    typedef std::map<int, ActionMapping> KeyToActionMap;

    KeyToActionMap            m_keyToActionMap;
    InputHandler            & m_inputHandler;
    bool                      m_captureMode;
    InputHandler::InputAction m_captureAction;
    int                       m_capturePlayer;
    SFX::Sound              * m_menuClick;
    SFX::Sound              * m_menuSelect;
};

#endif // EVENTHANDLER_HPP
