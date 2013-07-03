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

#ifndef MESSAGEOVERLAY_HPP
#define MESSAGEOVERLAY_HPP

#include "overlaybase.hpp"

#include <MCTextureText>
#include <QObject>
#include <QString>
#include <list>

class MCTextureFontManager;
class MCTextureFont;

//! A class that draws the radar, texts and displays messages.
class MessageOverlay : public QObject, public OverlayBase
{
    Q_OBJECT

public:

    //! Alignment of the MessageOverlay messages.
    enum MESSAGE_ALIGN {MA_TOP, MA_BOTTOM};

    //! Constructor.
    MessageOverlay(
        MessageOverlay::MESSAGE_ALIGN align = MA_BOTTOM, int messageMaxTime = 180);

    //! \reimp
    virtual void render();

    //! \reimp
    virtual bool update();

public slots:

    //! Add a message to the MessageOverlay.
    void addMessage(const std::string & msg);

    //! Add a message to the MessageOverlay.
    void addMessage(QString msg);

    //! Add a message to the MessageOverlay.
    void addMessage(const char * msg);

private:

    //! Render messages.
    void renderMessages();

    //! MessageOverlay message struct.
    struct Message
    {
        //! Time the message has been shown
        int timeShown;

        //! The time after which message will be deleted
        int maxTime;

        //! Current y
        float y;

        //! Target y
        float targetY;

        //! Message
        std::string text;

        //! True, if y is initialized
        bool isYInitialized;

        bool isRemoving;
    };

    MCTextureFontManager & m_fontManager;
    MCTextureFont        & m_font;
    MCTextureText          m_text;
    int                    m_messageMaxTime;
    MESSAGE_ALIGN          m_align;
    std::list<Message>     m_listMessages;
};

#endif // MESSAGEOVERLAY_HPP
