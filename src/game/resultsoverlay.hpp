// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef RESULTSOVERLAY_HPP
#define RESULTSOVERLAY_HPP

#include "overlaybase.hpp"
#include "race.hpp"

#include <MCGLColor>

#include <QString>

#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

class MCSurface;
class MCTextureFont;

//! Shows the results of the race after the human players have finished.
class ResultsOverlay : public OverlayBase
{
public:
    ResultsOverlay();

    void setRace(std::shared_ptr<Race> race);

    //! Loads the names of the human players. Call when a new race starts.
    void loadPlayerNames();

    //! Restarts the animations. Call when a new race starts.
    void reset() override;

    //! Advances the animations.
    void stepTime(std::chrono::milliseconds timeStep);

    //! Renders the full screen results table.
    void render() override;

    /*! Renders a card telling that the given human player has finished and waits for the other one.
     *  \param width Width of the player's split view.
     *  \param height Height of the player's split view. */
    void renderWaitingCard(size_t player, int width, int height);

private:
    enum class Align
    {
        Left,
        Center
    };

    /*! \return Eased animation progress from 0.0 to 1.0 of an element that starts to animate at the given time.
     *  \param elapsed Time since the animation of the whole view started. */
    static float progress(std::chrono::milliseconds elapsed, std::chrono::milliseconds start);

    void renderPanel(float x, float y, float width, float height, const MCGLColor & color);

    //! Text is rendered without shadow until it is fully opaque, because shadows can't fade.
    void renderText(const std::wstring & text, float x, float y, float size, const MCGLColor & color, Align align = Align::Left, float alpha = 1.0f);

    void renderTitle(float centerX, float & y, float lineHeight);

    void renderHeader(float x, float panelWidth, float & y, float lineHeight);

    void renderRows(const std::vector<Race::Result> & results, float x, float panelWidth, float & y, float lineHeight);

    void renderAchievements(float centerX, float & y, float lineHeight, size_t rowCount);

    std::wstring driverName(const Race::Result & result) const;

    std::wstring timeText(const Race::Result & result) const;

    std::wstring gapText(const Race::Result & result, const Race::Result & winner) const;

    std::shared_ptr<MCSurface> m_panel;

    MCTextureFont & m_font;

    std::shared_ptr<Race> m_race;

    std::array<QString, 2> m_playerNames;

    std::vector<std::wstring> m_positionTexts;

    //! Time since all human players finished and the results view started to animate.
    std::chrono::milliseconds m_elapsed { 0 };

    //! Time since each human player finished, for the waiting cards.
    std::array<std::chrono::milliseconds, 2> m_cardElapsed {};
};

#endif // RESULTSOVERLAY_HPP
