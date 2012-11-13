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

#include "trackselectionmenu.hpp"

#include "renderer.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "statemachine.hpp"
#include "timing.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <MenuItem>

#include <MCLogger>
#include <MCSurface>
#include <MCSurfaceManager>
#include <MCTextureFont>
#include <MCTextureFontManager>
#include <MCTextureText>

#include "../common/config.hpp"
#include "../common/mapbase.hpp"

#include <cassert>
#include <sstream>

class TrackItem : public MTFH::MenuItem
{
public:

    TrackItem(int width, int height, Track & track)
    : MenuItem(width, height)
    , m_track(track)
    , m_monospace(MCTextureFontManager::instance().font("default"))
    , m_star(MCSurfaceManager::instance().surface("star"))
    , m_glow(MCSurfaceManager::instance().surface("starGlow"))
    , m_lock(MCSurfaceManager::instance().surface("lock"))
    , m_xDisplacement(-1000)
    , m_lapRecord(Settings::instance().loadLapRecord(m_track))
    , m_bestPos(Settings::instance().loadBestPos(m_track))
    {
        m_star.setShaderProgram(&Renderer::instance().program("menu"));
        m_glow.setShaderProgram(&Renderer::instance().program("menu"));
        m_lock.setShaderProgram(&Renderer::instance().program("menu"));
    }

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

    virtual void setFocused(bool focused)
    {
        MenuItem::setFocused(focused);

        m_lapRecord = Settings::instance().loadLapRecord(m_track);
        m_bestPos   = Settings::instance().loadBestPos(m_track);
    }

    //! \reimp
    virtual void render(int x, int y);

private:

    Track         & m_track;
    MCTextureFont & m_monospace;
    MCSurface     & m_star;
    MCSurface     & m_glow;
    MCSurface     & m_lock;
    int             m_xDisplacement;
    int             m_lapRecord;
    int             m_bestPos;
};

// TODO: Split this fugly monster function into subs.
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
    int initX, initY;
    if (rMap.cols() % 2 == 0)
    {
        initX = x - rMap.cols() * tileW / 2 + tileW / 4 + m_xDisplacement;
    }
    else
    {
        initX = x - rMap.cols() * tileW / 2 + m_xDisplacement;
    }

    initY = y - rMap.rows() * tileH / 2;

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
                    pSurface->setShaderProgram(&Renderer::instance().program("menu"));
                    pSurface->bindTexture();

                    if (m_track.trackData().isLocked())
                    {
                        pSurface->setColor(0.5, 0.5, 0.5);
                    }
                    else
                    {
                        pSurface->setColor(1.0, 1.0, 1.0);
                    }

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

    const int textX   = x - width() / 2;
    const int shadowY = -2;
    const int shadowX =  2;

    // Render title
    std::stringstream ss;
    ss << m_track.trackData().name().toStdString();
    text.setText(ss.str());
    text.setGlyphSize(20, 20);
    text.setShadowOffset(shadowX, shadowY);
    text.render(x - text.width() / 2, y + height() / 2 + text.height(), nullptr, m_monospace);

    // Render stars
    if (!m_track.trackData().isLocked())
    {
        const int starW  = m_star.width();
        const int starH  = m_star.height();
        const int startX = x - 5 * starW + starW / 2 + m_xDisplacement;
        for (int i = 0; i < 10; i++)
        {
            if (m_bestPos != -1 && 10 - i >= m_bestPos)
            {
                m_star.setColor(1.0, 1.0, 0.0);
                m_glow.render(
                    nullptr,
                    MCVector3dF(startX + i * starW, y - height() / 2 + starH / 2, 0), 0);
            }
            else
            {
                m_star.setColor(0.75, 0.75, 0.75);
            }

            m_star.render(
                nullptr,
                MCVector3dF(startX + i * starW, y - height() / 2 + starH / 2, 0), 0);
        }
    }

    // Render the lock
    if (m_track.trackData().isLocked())
    {
        m_lock.render(
            nullptr,
            MCVector3dF(x + m_xDisplacement, y, 0), 0);
    }

    // Render track properties
    ss.str("");
    ss << "  Laps: " << m_track.trackData().lapCount();
    text.setText(ss.str());
    text.setGlyphSize(20, 20);
    text.render(textX, y - height() / 2 - text.height() * 2, nullptr, m_monospace);

    ss.str("");
    ss << "Length: "
       << int(m_track.trackData().route().geometricLength() * MCWorld::metersPerPixel())
       << " m";
    text.setText(ss.str());
    text.render(textX, y - height() / 2 - text.height() * 3, nullptr, m_monospace);

    if (!m_track.trackData().isLocked())
    {
        ss.str("");
        ss << "Record: " << Timing::msecsToString(m_lapRecord);
        text.setText(ss.str());
        text.render(textX, y - height() / 2 - text.height() * 4, nullptr, m_monospace);
    }

    text.setText("Use arrows to browse and enter to select..");
    text.setGlyphSize(10, 10);
    text.render(10, text.height(), nullptr, m_monospace);
}

TrackSelectionMenu::TrackSelectionMenu(std::string id,
    int width, int height, Scene & scene, StateMachine & sm)
: SurfaceMenu("trackSelectionBack", id, width, height, Menu::MS_SHOW_ONE)
, m_selectedTrack(nullptr)
, m_scene(scene)
, m_sm(sm)
{
    setWrapAround(false);
}

void TrackSelectionMenu::addTrack(Track & track)
{
    addItem(*new TrackItem(width() / 2, height() / 2, track), true);
    setCurrentIndex(0);
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

void TrackSelectionMenu::up()
{
    Menu::up();
    currentItem()->onRight();
}

void TrackSelectionMenu::down()
{
    Menu::down();
    currentItem()->onLeft();
}

void TrackSelectionMenu::selectCurrentItem()
{
    Menu::selectCurrentItem();
    Track & selection = static_cast<TrackItem *>(currentItem())->track();
    if (!selection.trackData().isLocked())
    {
        m_selectedTrack = &selection;
        m_sm.setTrack(*m_selectedTrack);
        m_scene.setActiveTrack(*m_selectedTrack);
        MCLogger().info() <<
            "Track '" << m_selectedTrack->trackData().name().toStdString() << "' selected.";
        setDone(true);
    }
}

Track * TrackSelectionMenu::selectedTrack() const
{
    return m_selectedTrack;
}
