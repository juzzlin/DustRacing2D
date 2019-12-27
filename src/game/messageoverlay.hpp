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
    enum class Alignment
    {
        Top,
        Bottom
    };

    //! Constructor.
    MessageOverlay(Alignment align = Alignment::Bottom, size_t messageMaxTime = 180);

    //! \reimp
    virtual void render() override;

    //! \reimp
    virtual bool update() override;

public slots:

    //! Add a message to the MessageOverlay.
    void addMessage(const std::wstring & msg);

    //! Add a message to the MessageOverlay.
    void addMessage(QString msg);

    //! Add a message to the MessageOverlay.
    void addMessage(const wchar_t * msg);

private:
    //! Render messages.
    void renderMessages();

    //! MessageOverlay message struct.
    struct Message
    {
        //! Time the message has been shown
        size_t timeShown = 0;

        //! The time after which message will be deleted
        size_t maxTime = 0;

        //! Current y
        float y = 0;

        //! Target y
        float targetY = 0;

        //! Message
        std::wstring text;

        //! True, if y is initialized
        bool isYInitialized = false;

        bool isRemoving = false;
    };

    MCTextureFontManager & m_fontManager;

    MCTextureFont & m_font;

    MCTextureText m_text;

    size_t m_messageMaxTime;

    Alignment m_align;

    std::list<Message> m_listMessages;
};

#endif // MESSAGEOVERLAY_HPP
