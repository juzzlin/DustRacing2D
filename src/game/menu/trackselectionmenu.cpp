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

#include "database.hpp"
#include "game.hpp"
#include "mainmenu.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "timing.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <MenuItem>
#include <MenuManager>

#include <MCAssetManager>
#include <MCSurface>
#include <MCTextureFont>
#include <MCTextureText>

#include "../common/config.hpp"
#include "../common/mapbase.hpp"

#include <cassert>
#include <memory>
#include <sstream>

#include <QObject> // For QObject::tr()

const float SAIL_AWAY_HONEY_X = 1000;

const int ANIMATION_STEPS = 15;

const int ANIMATION_EXP = 3;

std::string TrackSelectionMenu::MenuId = "trackSelection";

class TrackItem : public MTFH::MenuItem
{
public:
    TrackItem(int width, int height, std::shared_ptr<Track> track)
      : MenuItem(width, height)
      , m_game(Game::instance())
      , m_track(track)
      , m_font(MCAssetManager::textureFontManager().font(m_game.fontName()))
      , m_star(MCAssetManager::surfaceManager().surface("star"))
      , m_glow(MCAssetManager::surfaceManager().surface("starGlow"))
      , m_star_half(MCAssetManager::surfaceManager().surface("starHalf"))
      , m_star_half_r(MCAssetManager::surfaceManager().surface("starHalfR"))
      , m_glow_half(MCAssetManager::surfaceManager().surface("starHalfGlow"))
      , m_lock(MCAssetManager::surfaceManager().surface("lock"))
    {
        updateData();

        auto && program = Renderer::instance().program("menu");
        m_star->setShaderProgram(program);
        m_glow->setShaderProgram(program);
        m_star_half->setShaderProgram(program);
        m_star_half_r->setShaderProgram(program);
        m_glow_half->setShaderProgram(program);
        m_lock->setShaderProgram(program);
    }

    std::shared_ptr<Track> track() const
    {
        return m_track;
    }

    // This ensures that data has been updated when player returns to the menu after a race
    virtual void setFocused(bool focused) override
    {
        MenuItem::setFocused(focused);
        if (focused)
        {
            updateData();
        }
    }

    //! \reimp
    virtual void render() override;

private:
    void updateData();

    void renderTiles();

    void renderTitle();

    void renderStars();

    void renderLock();

    void renderTrackProperties();

    Game & m_game;

    std::shared_ptr<Track> m_track;

    MCTextureFont & m_font;

    MCSurfacePtr m_star;

    MCSurfacePtr m_glow;

    MCSurfacePtr m_star_half;

    MCSurfacePtr m_star_half_r;

    MCSurfacePtr m_glow_half;

    MCSurfacePtr m_lock;

    int m_lapRecord;

    int m_raceRecord;

    int m_bestPos;
};

void TrackItem::updateData()
{
    const int notSet = -1;
    const auto lapRecord = Database::instance().loadLapRecord(*m_track);
    m_lapRecord = lapRecord.second ? lapRecord.first : notSet;
    const auto raceRecord = Database::instance().loadRaceRecord(
      *m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty());
    m_raceRecord = raceRecord.second ? raceRecord.first : notSet;
    const auto bestPos = Database::instance().loadBestPos(
      *m_track, m_game.lapCount(), m_game.difficultyProfile().difficulty());
    m_bestPos = bestPos.second ? bestPos.first : notSet;
}

void TrackItem::renderTiles()
{
    const MapBase & rMap = m_track->trackData().map();

    const auto previewW = width();
    const auto previewH = height();

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
    float initX;
    if (rMap.cols() % 2 == 0)
    {
        initX = x() - rMap.cols() * tileW / 2 + tileW / 4 + menu()->x();
    }
    else
    {
        initX = x() - rMap.cols() * tileW / 2 + menu()->x();
    }

    // Loop through the visible tile matrix and draw the tiles
    float tileY = y() - rMap.rows() * tileH / 2 + menu()->y();
    for (size_t j = 0; j < rMap.rows(); j++)
    {
        float tileX = initX;
        for (size_t i = 0; i < rMap.cols(); i++)
        {
            const auto tile = std::static_pointer_cast<TrackTile>(rMap.getTile(i, j));
            const auto surface = tile->previewSurface();
            if (surface && !tile->excludeFromMinimap())
            {
                surface->setShaderProgram(Renderer::instance().program("menu"));
                surface->bind();

                if (m_track->trackData().isLocked())
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
                  MCVector3dF(tileX + tileW / 2, tileY + tileH / 2), tile->rotation());
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
    const int shadowX = 2;

    std::wstringstream ss;
    ss << m_track->trackData().name().toUpper().toStdWString();
    text.setText(ss.str());
    text.setGlyphSize(30, 30);
    text.setShadowOffset(shadowX, shadowY);
    text.render(menu()->x() + x() - text.width(m_font) / 2, menu()->y() + y() + height() / 2 + text.height(m_font), nullptr, m_font);
}

void TrackItem::renderStars()
{
    if (!m_track->trackData().isLocked())
    {
        const auto starW = m_star->width();
        const auto starH = m_star->height();
        const auto startX = menu()->x() + x() - 5 * starW + starW / 2;
        const MCGLColor yellow(1.0, 1.0, 0.0);
        const MCGLColor grey(.75, .75, .75);

        const int numStars = 10;
        for (int i = 0; i < numStars; i++)
        {
            const MCVector3dF starPos(startX + i * starW, menu()->y() + y() - height() / 2 + starH / 2, 0);

            // The range of m_bestPos is 1..NUM_CARS
            if (m_bestPos != -1 && numStars - i >= m_bestPos)
            {
                m_star->setColor(yellow);
                m_glow->render(nullptr, starPos, 0);
                m_star->render(nullptr, starPos, 0);
            }
            // Award a half star for rank 11
            else if (m_bestPos == 11 && i == 0)
            {
                m_star_half->setColor(yellow);
                m_star_half_r->setColor(grey);
                m_glow_half->render(nullptr, starPos, 0);
                m_star_half_r->render(nullptr, starPos, 0);
                m_star_half->render(nullptr, starPos, 0);
            }
            else
            {
                m_star->setColor(grey);
                m_star->render(nullptr, starPos, 0);
            }
        }
    }
}

void TrackItem::renderLock()
{
    if (m_track->trackData().isLocked())
    {
        m_lock->render(nullptr, { menu()->x() + x(), menu()->y() + y(), 0 }, 0);
    }
}

void TrackItem::renderTrackProperties()
{
    std::wstringstream ss;
    MCTextureText text(L"");
    text.setGlyphSize(20, 20);
    const int shadowY = -2;
    const int shadowX = 2;
    text.setShadowOffset(shadowX, shadowY);

    std::vector<MCTextureText> texts;

    // Render track properties
    ss.str(L"");
    ss << QObject::tr("       Laps: ").toStdWString() << Game::instance().lapCount();
    text.setText(ss.str());
    text.setGlyphSize(20, 20);
    float maxWidth = text.width(m_font);
    texts.push_back(text);

    ss.str(L"");
    ss << QObject::tr("     Length: ").toStdWString()
       << static_cast<int>(m_track->trackData().route().geometricLength() * MCWorld::metersPerUnit());
    text.setText(ss.str());
    maxWidth = std::fmax(maxWidth, text.width(m_font));
    texts.push_back(text);

    ss.str(L"");
    ss << QObject::tr(" Lap Record: ").toStdWString() << Timing::msecsToString(m_lapRecord);
    text.setText(ss.str());
    maxWidth = std::fmax(maxWidth, text.width(m_font));
    if (!m_track->trackData().isLocked())
        texts.push_back(text);

    ss.str(L"");
    ss << QObject::tr("Race Record: ").toStdWString() << Timing::msecsToString(m_raceRecord);
    text.setText(ss.str());
    maxWidth = std::fmax(maxWidth, text.width(m_font));
    if (!m_track->trackData().isLocked())
        texts.push_back(text);

    const float yPos = menu()->y() + y() - height() / 2;
    const float lineHeight = text.height(m_font);
    int line = 2;
    for (auto && text : texts)
    {
        const auto textX = menu()->x() + x();
        text.render(textX - maxWidth / 2, yPos - lineHeight * line, nullptr, m_font);
        line++;
    }

    line++;

    if (m_track->trackData().isLocked())
    {
        ss.str(L"");
        if (m_game.hasComputerPlayers())
        {
            //: Try to keep the translation as short as possible.
            ss << QObject::tr("Finish previous track in TOP-6 to unlock!").toStdWString();
        }
        else
        {
            //: "it" = a locked track. Try to keep the translation as short as possible.
            ss << QObject::tr("Unlock it in one/two player race!").toStdWString();
        }
        text.setText(ss.str());
        maxWidth = std::fmax(maxWidth, text.width(m_font));
        const auto textX = menu()->x() + x();
        text.render(textX - maxWidth / 2, yPos - lineHeight * line, nullptr, m_font);
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

TrackSelectionMenu::TrackSelectionMenu(int width, int height, Scene & scene)
  : SurfaceMenu("trackSelectionBack", MenuId, width, height, Menu::Style::ShowMany, true, true, true)
  , m_scene(scene)
{
    setWrapAround(false);
}

void TrackSelectionMenu::addTrack(std::shared_ptr<Track> track)
{
    const auto item = std::make_shared<TrackItem>(width() / 2, height() / 2, track);
    item->setPos(width() / 2, height() / 2);
    addItem(item);
    setCurrentIndex(0);
    setItemsToShow({ 0 });
}

void TrackSelectionMenu::left()
{
    const int prevIndex = currentIndex();

    if (prevIndex > 0)
    {
        currentItem()->setPos(width() / 2, height() / 2, width() + SAIL_AWAY_HONEY_X, height() / 2);
        currentItem()->resetAnimationCurve(ANIMATION_STEPS, ANIMATION_EXP);

        Menu::left();

        currentItem()->setPos(-SAIL_AWAY_HONEY_X, height() / 2, width() / 2, height() / 2);
        currentItem()->resetAnimationCurve(ANIMATION_STEPS, ANIMATION_EXP);

        setItemsToShow({ prevIndex, currentIndex() });
    }
}

void TrackSelectionMenu::right()
{
    const int prevIndex = currentIndex();

    if (prevIndex + 1 < static_cast<int>(itemCount()))
    {
        currentItem()->setPos(width() / 2, height() / 2, -SAIL_AWAY_HONEY_X, height() / 2);
        currentItem()->resetAnimationCurve(ANIMATION_STEPS, ANIMATION_EXP);

        Menu::right();

        currentItem()->setPos(width() + SAIL_AWAY_HONEY_X, height() / 2, width() / 2, height() / 2);
        currentItem()->resetAnimationCurve(ANIMATION_STEPS, ANIMATION_EXP);

        setItemsToShow({ prevIndex, currentIndex() });
    }
}

void TrackSelectionMenu::up()
{
    left();
}

void TrackSelectionMenu::down()
{
    right();
}

void TrackSelectionMenu::exit()
{
    MTFH::MenuManager::instance().popToMenu(MainMenu::MenuId);
}

void TrackSelectionMenu::selectCurrentItem()
{
    Menu::selectCurrentItem();
    auto && selectedTrack = std::static_pointer_cast<TrackItem>(currentItem())->track();
    if (!selectedTrack->trackData().isLocked())
    {
        m_selectedTrack = selectedTrack;
        m_scene.setActiveTrack(m_selectedTrack);
        setIsDone(true);
    }
}

std::shared_ptr<Track> TrackSelectionMenu::selectedTrack() const
{
    return m_selectedTrack;
}
