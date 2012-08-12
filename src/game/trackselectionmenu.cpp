// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "trackselectionmenu.hpp"

#include "renderer.hpp"
#include "scene.hpp"
#include "statemachine.hpp"
#include "timing.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
#include "menuitem.hpp"

#include <MCLogger>
#include <MCSurface>
#include <MCTextureManager>
#include <MCTextureFont>
#include <MCTextureFontManager>
#include <MCTextureText>

#include "../common/config.hpp"
#include "../common/mapbase.hpp"

#include <QSettings>

#include <cassert>
#include <sstream>

namespace
{
    const char * SETTINGS_GROUP = "LapRecords";
}

class TrackItem : public MenuItem
{
public:

    TrackItem(int width, int height, Track & track)
    : MenuItem(width, height)
    , m_track(track)
    , m_xDisplacement(-1000)
    {}

    Track & track() const
    {
        return m_track;
    }

    virtual void onLeft()
    {
        m_xDisplacement = -1000;
    }

    virtual void onRight()
    {
        m_xDisplacement = 1000;
    }

    //! \reimp
    virtual void render(int x, int y);

private:

    Track & m_track;
    int m_xDisplacement;
};

int loadLapRecord(std::string trackName)
{
    // Open settings file
    QSettings settings(Config::Common::QSETTINGS_COMPANY_NAME,
        Config::Game::QSETTINGS_SOFTWARE_NAME);

    // Read record time, -1 if not found
    settings.beginGroup(SETTINGS_GROUP);
    const int time = settings.value(trackName.c_str(), -1).toInt();
    settings.endGroup();

    return time;
}

void TrackItem::render(int x, int y)
{
    const MapBase & rMap = m_track.trackData().map();

    const int previewW = width();
    const int previewH = height();

    m_xDisplacement = m_xDisplacement * 2 / 3;

    // Set tileW and tileH so that they are squares
    MCFloat tileW = previewW / rMap.cols();
    MCFloat tileH = previewH / rMap.rows();

    if (tileW > tileH)
    {
        tileW = tileH;
    }
    else
    {
        tileH = tileW;
    }

    // Center the preview
    const int initX = x - rMap.cols() * tileW / 2 + m_xDisplacement;
    const int initY = y - rMap.rows() * tileH / 2;

    // Loop through the visible tile matrix and draw the tiles
    MCFloat tileX, tileY;
    tileY = initY;
    const int j2 = rMap.rows();
    const int i2 = rMap.cols();
    for (int j = 0; j <= j2; j++)
    {
        tileX = initX;
        for (int i = 0; i <= i2; i++)
        {
            if (TrackTile * pTile = static_cast<TrackTile *>(rMap.getTile(i, j)))
            {
                if (MCSurface * pSurface = pTile->previewSurface())
                {
                    pSurface->setShaderProgram(&Renderer::instance().masterProgram());
                    pSurface->renderScaled(
                        nullptr,
                        MCVector3dF(tileX + tileW / 2, tileY + tileH / 2, std::abs(m_xDisplacement)),
                        tileW / 2, tileH / 2,
                        pTile->rotation());
                }
            }

            tileX += tileW;
        }

        tileY += tileH;
    }

    MCTextureText text("");
    MCTextureFont defaultMonospace = MCTextureFontManager::instance().font("default");

    const int textX   = x - width() / 2;
    const int shadowY = -2;
    const int shadowX =  2;

    {
        std::stringstream ss;
        ss << m_track.trackData().name().toStdString();
        text.setText(ss.str());
        text.setGlyphSize(20, 20);
        text.setShadowOffset(shadowX, shadowY);
        text.render(
            x - text.textWidth() / 2, y + height() / 2 + text.textHeight(),
            nullptr, defaultMonospace);
    }

    {
        std::stringstream ss;
        ss << "  Laps: " << m_track.trackData().lapCount();
        text.setText(ss.str());
        text.setGlyphSize(20, 20);
        text.render(textX, y - height() / 2 - text.textHeight(), nullptr, defaultMonospace);
    }

    {
        std::stringstream ss;
        ss << "Length: "
           << int(m_track.trackData().route().geometricLength() * MCWorld::metersPerPixel())
           << " m";
        text.setText(ss.str());
        text.render(textX, y - height() / 2 - text.textHeight() * 2, nullptr, defaultMonospace);
    }

    {
        std::stringstream ss;
        ss << "Record: "
           << Timing::msecsToString(loadLapRecord(m_track.trackData().name().toStdString()));
        text.setText(ss.str());
        text.render(textX, y - height() / 2 - text.textHeight() * 3, nullptr, defaultMonospace);
    }

    {
        text.setText("Use arrows to browse and enter to select..");
        text.setGlyphSize(10, 10);
        text.render(10, text.textHeight(), nullptr, defaultMonospace);
    }
}

TrackSelectionMenu::TrackSelectionMenu(std::string id,
    int width, int height, Scene & scene, StateMachine & sm)
: Menu(id, width, height, Menu::MS_SHOW_ONE)
, m_selectedTrack(nullptr)
, m_scene(scene)
, m_sm(sm)
{
}

void TrackSelectionMenu::addTrack(Track & track)
{
    addItem(*new TrackItem(width() / 2, height() / 2, track), true);
    setCurrentIndex(0);
}

void TrackSelectionMenu::render()
{
    MCSurface & back = MCTextureManager::instance().surface("menu");
    back.setShaderProgram(&Renderer::instance().masterProgram());
    back.renderScaled(nullptr, MCVector3dF(0, 0, 0), width(), height(), 0);
    Menu::render();
}

void TrackSelectionMenu::left()
{
    Menu::left();
    currentItem()->onLeft();
}

void TrackSelectionMenu::right()
{
    Menu::right();
    currentItem()->onRight();
}

void TrackSelectionMenu::selectCurrentItem()
{
    Menu::selectCurrentItem();
    m_selectedTrack = &static_cast<TrackItem *>(currentItem())->track();
    m_sm.setTrack(*m_selectedTrack);
    m_scene.setActiveTrack(*m_selectedTrack);
    MCLogger().info() <<
        "Track '" << m_selectedTrack->trackData().name().toStdString() << "' selected.";
    setDone(true);
}

Track * TrackSelectionMenu::selectedTrack() const
{
    return m_selectedTrack;
}
