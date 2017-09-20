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

#include "messageoverlay.hpp"
#include "game.hpp"
#include <MCCamera>
#include <MCAssetManager>

static const int GLYPH_WIDTH  = 20;
static const int GLYPH_HEIGHT = 20;

MessageOverlay::MessageOverlay(Alignment align, int messageMaxTime)
: m_fontManager(MCAssetManager::textureFontManager())
, m_font(m_fontManager.font(Game::instance().fontName()))
, m_text(L"")
, m_messageMaxTime(messageMaxTime)
, m_align(align)
{
    m_text.setShadowOffset(2, -2);
    m_text.setGlyphSize(GLYPH_WIDTH, GLYPH_HEIGHT);
}

void MessageOverlay::render()
{
    renderMessages();
}

bool MessageOverlay::update()
{
    // Loop trough active messages, increment their time and
    // remove messages when needed.
    auto i(m_listMessages.begin());
    while (i != m_listMessages.end())
    {
        MessageOverlay::Message & m = *i;

        // Inc time
        m.timeShown++;

        // Animate y
        if (m.isYInitialized)
        {
            m.y = m.y + (m.targetY - m.y) / 4;
        }

        // Time to vanish?
        if (m.timeShown >= m.maxTime / 2)
        {
            m.targetY    = -20;
            m.isRemoving = true;
        }

        if (m.timeShown >= m.maxTime)
        {
            // Yes: remove from the list
            i = m_listMessages.erase(i);
        }
        else
        {
            i++;
        }
    }

    return false;
}

void MessageOverlay::addMessage(const std::wstring & msg)
{
    // Create a new message
    MessageOverlay::Message myMsg;

    myMsg.timeShown      = 0;
    myMsg.maxTime        = m_messageMaxTime * 2; // Half of the time is used when removing
    myMsg.text           = msg;
    myMsg.y              = -1;
    myMsg.targetY        = -1;
    myMsg.isYInitialized = false;
    myMsg.isRemoving     = false;

    // Add to the list of active messages
    m_listMessages.push_front(myMsg);
}

void MessageOverlay::addMessage(QString msg)
{
    addMessage(msg.toStdWString());
}

void MessageOverlay::addMessage(const wchar_t * msg)
{
    addMessage(std::wstring(msg));
}

void MessageOverlay::renderMessages()
{
    // Text coordinates
    int x, y;

    // Y delta
    int dY;

    if (m_align == MessageOverlay::Alignment::Bottom)
    {
        // Y-coordinate for the fist message
        y  = GLYPH_HEIGHT;
        dY = GLYPH_HEIGHT;
    }
    else
    {
        // Y-coordinate for the fist message
        y  = height() - 2 * GLYPH_HEIGHT;
        dY = -GLYPH_HEIGHT;
    }

    // Center X-coordinate
    x = width() / 2;

    // Loop through active messages and init their targetY
    auto i(m_listMessages.begin());
    while (i != m_listMessages.end())
    {
        MessageOverlay::Message & m = *i;

        if (!m.isYInitialized)
        {
            m.y = y;
            m.targetY = y;
            m.isYInitialized = true;
        }
        else
        {
            if (!m.isRemoving)
            {
                m.targetY = y;
            }
        }

        // Increment / decrement y-coordinate
        y += dY;

        i++;
    }

    // Loop trough active messages and render them stacked
    auto j(m_listMessages.begin());
    while (j != m_listMessages.end())
    {
        const MessageOverlay::Message & m = *j;

        // Create a Text object from the std QString
        m_text.setText(m.text);

        // Render the text
        m_text.render(x - (m_text.width(m_font) / 2), m.y, nullptr, m_font);

        j++;
    }
}
