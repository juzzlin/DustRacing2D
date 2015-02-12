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

#include "game.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "renderer.hpp"
#include "timing.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <MenuItem>

#include <MCAssetManager>
#include <MCLogger>
#include <MCSurface>
#include <MCTextureFont>
#include <MCTextureText>

#include "../common/config.hpp"
#include "../common/mapbase.hpp"

#include <cassert>
#include <sstream>

#include <QObject> // For QObject::tr()

class TrackItem : public MTFH::MenuItem
{
public:

    TrackItem(int width, int height, Track & track)
    : MenuItem(width, height)
    , m_track(track)
    , m_monospace(MCAssetManager::textureFontManager().font(Game::instance().fontName()))
    , m_star(MCAssetManager::surfaceManager().surface("star"))
    , m_glow(MCAssetManager::surfaceManager().surface("starGlow"))
    , m_lock(MCAssetManager::surfaceManager().surface("lock"))
    , m_xDisplacement(-1000)
    , m_lapRecord(Settings::instance().loadLapRecord(m_track))
    , m_raceRecord(Settings::instance().loadRaceRecord(m_track, Game::instance().lapCount()))
    , m_bestPos(Settings::instance().loadBestPos(m_track, Game::instance().lapCount()))
    {
        m_star.setShaderProgram(Renderer::instance().program("menu"));
        m_glow.setShaderProgram(Renderer::instance().program("menu"));
        m_lock.setShaderProgram(Renderer::instance().program("menu"));
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

        m_lapRecord  = Settings::instance().loadLapRecord(m_track);
        m_raceRecord = Settings::instance().loadRaceRecord(m_track, Game::instance().lapCount());
        m_bestPos    = Settings::instance().loadBestPos(m_track, Game::instance().lapCount());
    }

    //! \reimp
    virtual void render();

private:

    Track         & m_track;
    MCTextureFont & m_monospace;
    MCSurface     & m_star;
    MCSurface     & m_glow;
    MCSurface     & m_lock;
    int             m_xDisplacement;
    int             m_lapRecord;
    int             m_raceRecord;
    int             m_bestPos;
};

// TODO: Split this fugly monster function into subs.
void TrackItem::render()
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
        initX = x() - rMap.cols() * tileW / 2 + tileW / 4 + m_xDisplacement;
    }
    else
    {
        initX = x() - rMap.cols() * tileW / 2 + m_xDisplacement;
    }

    initY = y() - rMap.rows() * tileH / 2;

    // Loop through the visible tile matrix and draw the tiles
    MCFloat tileX, tileY;
    tileY = initY;
    const int j2 = rMap.rows();
    const int i2 = rMap.cols();
    for (int j = 0; j < j2; j++)
    {
        tileX = initX;
        for (int i = 0; i < i2; i++)
        {
            if (TrackTile * pTile = static_cast<TrackTile *>(rMap.getTile(i, j)))
            {
                if (MCSurface * pSurface = pTile->previewSurface())
                {
                    pSurface->setShaderProgram(Renderer::instance().program("menu"));
                    pSurface->bindMaterial();

                    if (m_track.trackData().isLocked())
                    {
                        pSurface->setColor(MCGLColor(0.5, 0.5, 0.5));
                    }
                    else
                    {
                        pSurface->setColor(MCGLColor(1.0, 1.0, 1.0));
                    }

                    pSurface->setSize(tileH, tileW);
                    pSurface->render(
                        nullptr,
                        MCVector3dF(tileX + tileW / 2, tileY + tileH / 2, std::abs(m_xDisplacement)),
                        pTile->rotation());
                }
            }

            tileX += tileW;
        }

        tileY += tileH;
    }

    MCTextureText text(L"");

    const int textX   = x() - width() / 2;
    const int shadowY = -2;
    const int shadowX =  2;

    // Render title
    std::wstringstream ss;
    ss << m_track.trackData().name().toStdWString();
    text.setText(ss.str());
    text.setGlyphSize(20, 20);
    text.setShadowOffset(shadowX, shadowY);
    text.render(x() - text.width() / 2, y() + height() / 2 + text.height(), nullptr, m_monospace);

    // Render stars
    if (!m_track.trackData().isLocked())
    {
        const int starW    = m_star.width();
        const int starH    = m_star.height();
        const int startX   = x() - 5 * starW + starW / 2 + m_xDisplacement;
        const int numStars = 10;

        for (int i = 0; i < numStars; i++)
        {
            // The range of m_bestPos is 1..NUM_CARS
            if (m_bestPos != -1 && Scene::NUM_CARS - (i + 1 ) * Scene::NUM_CARS / numStars >= m_bestPos - 1)
            {
                m_star.setColor(MCGLColor(1.0, 1.0, 0.0));
                m_glow.render(
                    nullptr, MCVector3dF(startX + i * starW, y() - height() / 2 + starH / 2, 0), 0);
            }
            else
            {
                m_star.setColor(MCGLColor(0.75, 0.75, 0.75));
            }

            m_star.render(
                nullptr, MCVector3dF(startX + i * starW, y() - height() / 2 + starH / 2, 0), 0);
        }
    }

    // Render the lock
    if (m_track.trackData().isLocked())
    {
        m_lock.render(nullptr, MCVector3dF(x() + m_xDisplacement, y(), 0), 0);
    }

    // Render track properties
    ss.str(L"");
    ss << QObject::tr("       Laps: ").toStdWString() << Game::instance().lapCount();
    text.setText(ss.str());
    text.setGlyphSize(20, 20);
    text.render(textX, y() - height() / 2 - text.height() * 2, nullptr, m_monospace);

    ss.str(L"");
    ss << QObject::tr("     Length: ").toStdWString()
       << int(m_track.trackData().route().geometricLength() * MCWorld::metersPerUnit())
       << QObject::tr(" m").toStdWString();
    text.setText(ss.str());
    text.render(textX, y() - height() / 2 - text.height() * 3, nullptr, m_monospace);

    if (!m_track.trackData().isLocked())
    {
        ss.str(L"");
        ss << QObject::tr(" Lap Record: ").toStdWString() << Timing::msecsToString(m_lapRecord);
        text.setText(ss.str());
        text.render(textX, y() - height() / 2 - text.height() * 4, nullptr, m_monospace);

        ss.str(L"");
        ss << QObject::tr("Race Record: ").toStdWString() << Timing::msecsToString(m_raceRecord);
        text.setText(ss.str());
        text.render(textX, y() - height() / 2 - text.height() * 5, nullptr, m_monospace);
    }
}

TrackSelectionMenu::TrackSelectionMenu(std::string id,
    int width, int height, Scene & scene)
    : SurfaceMenu("trackSelectionBack", id, width, height, Menu::Style::ShowOne, true, true, true)
, m_selectedTrack(nullptr)
, m_scene(scene)
{
    setWrapAround(false);
}

void TrackSelectionMenu::addTrack(Track & track)
{
    addItem(MTFH::MenuItemPtr(new TrackItem(width() / 2, height() / 2, track)));
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
    Track & selection = static_cast<TrackItem *>(currentItem().get())->track();
    if (!selection.trackData().isLocked())
    {
        m_selectedTrack = &selection;
        m_scene.setActiveTrack(*m_selectedTrack);
        setIsDone(true);
    }
}

Track * TrackSelectionMenu::selectedTrack() const
{
    return m_selectedTrack;
}
