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

    //! Renders the full screen results table.
    void render() override;

private:
    enum class Align
    {
        Left,
        Center
    };

    void renderPanel(float x, float y, float width, float height, const MCGLColor & color);

    void renderText(const std::wstring & text, float x, float y, float size, const MCGLColor & color, Align align = Align::Left);

    void renderTitle(float centerX, float & y, float lineHeight);

    void renderHeader(float x, float panelWidth, float & y, float lineHeight);

    void renderRows(const std::vector<Race::Result> & results, float x, float panelWidth, float & y, float lineHeight);

    void renderAchievements(float centerX, float & y, float lineHeight);

    std::wstring driverName(const Race::Result & result) const;

    std::wstring timeText(const Race::Result & result) const;

    std::wstring gapText(const Race::Result & result, const Race::Result & winner) const;

    std::shared_ptr<MCSurface> m_panel;

    MCTextureFont & m_font;

    std::shared_ptr<Race> m_race;

    std::array<QString, 2> m_playerNames;

    std::vector<std::wstring> m_positionTexts;
};

#endif // RESULTSOVERLAY_HPP
