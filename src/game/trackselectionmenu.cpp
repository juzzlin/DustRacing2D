// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
#include <memory>
#include <sstream>

#include <QObject> // For QObject::tr()

class TrackItem : public MTFH::MenuItem
{
public:

    TrackItem(int width, int height, Track & track)
    : MenuItem(width, height)
    , m_game(Game::instance())
    , m_track(track)
    , m_font(MCAssetManager::textureFontManager().font(m_game.fontName()))
    , m_star(MCAssetManager::surfaceManager().surface("star"))
    , m_glow(MCAssetManager::surfaceManager().surface("starGlow"))
    , m_lock(MCAssetManager::surfaceManager().surface("lock"))
    , m_xDisplacement(-1000)
    , m_lapRecord(Settings::instance().loadLapRecord(m_track))
    , m_raceRecord(Settings::instance().loadRaceRecord(
        m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty()))
    , m_bestPos(Settings::instance().loadBestPos(
        m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty()))
    {
        m_star.setShaderProgram(Renderer::instance().program("default"));
        m_glow.setShaderProgram(Renderer::instance().program("default"));
        m_lock.setShaderProgram(Renderer::instance().program("default"));
    }

    Track & track() const
    {
        return m_track;
    }

    virtual void onLeft() override
    {
        m_xDisplacement = -1000;
    }

    virtual void onRight() override
    {
        m_xDisplacement = 1000;
    }

    virtual void setFocused(bool focused) override
    {
        MenuItem::setFocused(focused);

        m_lapRecord  = Settings::instance().loadLapRecord(m_track);
        m_raceRecord = Settings::instance().loadRaceRecord(
            m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty());
        m_bestPos = Settings::instance().loadBestPos(
            m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty());
    }

    //! \reimp
    virtual void render() override;

private:

    void renderTiles();

    void renderTitle();

    void renderStars();

    void renderLock();

    void renderTrackProperties();

    Game & m_game;

    Track & m_track;

    MCTextureFont & m_font;

    MCSurface & m_star;

    MCSurface & m_glow;

    MCSurface & m_lock;

    int m_xDisplacement;

    int m_lapRecord;

    int m_raceRecord;

    int m_bestPos;
};

void TrackItem::renderTiles()
{
    const MapBase & rMap = m_track.trackData().map();

    const int previewW = width();
    const int previewH = height();

    m_xDisplacement = m_xDisplacement * 2 / 3;

    // Set tileW and tileH so that they are squares
    float tileW = previewW / rMap.cols();
    float tileH = previewH / rMap.rows();

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
    float tileY = initY;
    for (unsigned int j = 0; j < rMap.rows(); j++)
    {
        float tileX = initX;
        for (unsigned int i = 0; i < rMap.cols(); i++)
        {
            auto tile = std::static_pointer_cast<TrackTile>(rMap.getTile(i, j));
            auto surface = tile->previewSurface();
            if (surface && !tile->excludeFromMinimap())
            {
                surface->setShaderProgram(Renderer::instance().program("default"));
                surface->bind();

                if (m_track.trackData().isLocked())
                {
                    surface->setColor(MCGLColor(0.5, 0.5, 0.5));
                }
                else
                {
                    surface->setColor(MCGLColor(1.0, 1.0, 1.0));
                }

                surface->setSize(tileH, tileW);
                surface->render(
                    nullptr,
                    MCVector3dF(tileX + tileW / 2, tileY + tileH / 2, std::abs(m_xDisplacement)),
                    tile->rotation());
            }

            tileX += tileW;
        }

        tileY += tileH;
    }
}

void TrackItem::renderTitle()
{
    MCTextureText text(L"");

    const int shadowY = -2;
    const int shadowX =  2;

    std::wstringstream ss;
    ss << m_track.trackData().name().toStdWString();
    text.setText(ss.str());
    text.setGlyphSize(20, 20);
    text.setShadowOffset(shadowX, shadowY);
    text.render(x() - text.width(m_font) / 2, y() + height() / 2 + text.height(m_font), nullptr, m_font);
}

void TrackItem::renderStars()
{
    if (!m_track.trackData().isLocked())
    {
        const int starW = m_star.width();
        const int starH = m_star.height();
        const int startX = x() - 5 * starW + starW / 2 + m_xDisplacement;
        const int numStars = 10;
        const MCGLColor yellow(1.0, 1.0, 0.0);
        const MCGLColor grey(.75, .75, .75);

        for (int i = 0; i < numStars; i++)
        {
            const MCVector3dF starPos(startX + i * starW, y() - height() / 2 + starH / 2, 0);

            // The range of m_bestPos is 1..NUM_CARS
            if (m_bestPos != -1 &&
                Scene::NUM_CARS - (i + 1 ) * Scene::NUM_CARS / numStars >= m_bestPos - 1)
            {
                m_star.setColor(yellow);
                m_glow.render(nullptr, starPos, 0);
            }
            else
            {
                m_star.setColor(grey);
            }

            m_star.render(nullptr, starPos, 0);
        }
    }
}

void TrackItem::renderLock()
{
    if (m_track.trackData().isLocked())
    {
        m_lock.render(nullptr, MCVector3dF(x() + m_xDisplacement, y(), 0), 0);
    }
}

void TrackItem::renderTrackProperties()
{
    MCTextureText text(L"");

    const int shadowY = -2;
    const int shadowX =  2;

    std::wstringstream ss;
    text.setGlyphSize(20, 20);
    text.setShadowOffset(shadowX, shadowY);

    const int textX = x() - width() / 2;

    // Render track properties
    ss.str(L"");
    ss << QObject::tr("       Laps: ").toStdWString() << Game::instance().lapCount();
    text.setText(ss.str());
    text.setGlyphSize(20, 20);
    text.render(textX, y() - height() / 2 - text.height(m_font) * 2, nullptr, m_font);

    ss.str(L"");
    ss << QObject::tr("     Length: ").toStdWString()
       << int(m_track.trackData().route().geometricLength() * MCWorld::metersPerUnit())
       << QObject::tr(" m").toStdWString();
    text.setText(ss.str());
    text.render(textX, y() - height() / 2 - text.height(m_font) * 3, nullptr, m_font);

    if (!m_track.trackData().isLocked())
    {
        ss.str(L"");
        ss << QObject::tr(" Lap Record: ").toStdWString() << Timing::msecsToString(m_lapRecord);
        text.setText(ss.str());
        text.render(textX, y() - height() / 2 - text.height(m_font) * 4, nullptr, m_font);

        ss.str(L"");
        ss << QObject::tr("Race Record: ").toStdWString() << Timing::msecsToString(m_raceRecord);
        text.setText(ss.str());
        text.render(textX, y() - height() / 2 - text.height(m_font) * 5, nullptr, m_font);
    }
}

void TrackItem::render()
{
    renderTiles();

    renderTitle();

    renderStars();

    renderLock();

    renderTrackProperties();
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
    auto && selection = std::static_pointer_cast<TrackItem>(currentItem())->track();
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
