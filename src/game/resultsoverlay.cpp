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

#include "resultsoverlay.hpp"

#include "game.hpp"
#include "settings.hpp"
#include "timing.hpp"
#include "track.hpp"
#include "trackdata.hpp"

#include <MCAssetManager>
#include <MCSurface>
#include <MCTextureFont>
#include <MCTextureText>

#include <QObject> // For QObject::tr()

#include <algorithm>

namespace {

//! Columns as fractions of the panel width.
const float positionColumn { 0.05f };
const float numberColumn { 0.13f };
const float driverColumn { 0.21f };
const float timeColumn { 0.50f };
const float gapColumn { 0.66f };
const float bestLapColumn { 0.81f };

//! Lines on the screen: title, subtitle, header, 12 cars, achievements and the footer with some spacing.
const float linesPerScreen { 24 };

//! Glyph size relative to the line height.
const float glyphScale { 0.6f };

const MCGLColor panelColor { 0.0f, 0.0f, 0.0f, 0.75f };
const MCGLColor humanRowColor { 0.25f, 0.75f, 1.0f, 0.25f };
const MCGLColor titleColor { 0.25f, 0.75f, 1.0f };
const MCGLColor headerColor { 0.6f, 0.6f, 0.6f };
const MCGLColor finishedColor { 1.0f, 1.0f, 1.0f };
const MCGLColor racingColor { 0.6f, 0.6f, 0.6f };
const MCGLColor achievementColor { 1.0f, 1.0f, 0.0f };

} // namespace

ResultsOverlay::ResultsOverlay()
  : m_panel { MCAssetManager::surfaceManager().surface("panel") }
  , m_font { MCAssetManager::textureFontManager().font(Game::instance().fontName()) }
  , m_positionTexts {
      QObject::tr("---").toStdWString(),
      QObject::tr("1st").toStdWString(),
      QObject::tr("2nd").toStdWString(),
      QObject::tr("3rd").toStdWString(),
      QObject::tr("4th").toStdWString(),
      QObject::tr("5th").toStdWString(),
      QObject::tr("6th").toStdWString(),
      QObject::tr("7th").toStdWString(),
      QObject::tr("8th").toStdWString(),
      QObject::tr("9th").toStdWString(),
      QObject::tr("10th").toStdWString(),
      QObject::tr("11th").toStdWString(),
      QObject::tr("12th").toStdWString()
  }
{
    loadPlayerNames();
}

void ResultsOverlay::setRace(std::shared_ptr<Race> race)
{
    m_race = race;
}

void ResultsOverlay::loadPlayerNames()
{
    for (size_t player = 0; player < m_playerNames.size(); player++)
    {
        m_playerNames.at(player) = Settings::instance().loadPlayerName(player);
    }
}

void ResultsOverlay::renderPanel(float x, float y, float width, float height, const MCGLColor & color)
{
    m_panel->setColor(color);
    m_panel->setSize(width, height);
    m_panel->render(nullptr, { x + width / 2, y + height / 2, 0 }, 0);
}

void ResultsOverlay::renderText(const std::wstring & text, float x, float y, float size, const MCGLColor & color, Align align)
{
    MCTextureText textureText { text };
    textureText.setGlyphSize(size, size);
    textureText.setColor(color);
    textureText.setShadowOffset(2, -2);
    const float alignedX { align == Align::Center ? x - textureText.width(m_font) / 2 : x };
    textureText.render(alignedX, y, nullptr, m_font);
}

std::wstring ResultsOverlay::driverName(const Race::Result & result) const
{
    if (result.isHuman && result.carIndex < m_playerNames.size())
    {
        return m_playerNames.at(result.carIndex).toStdWString();
    }

    return QObject::tr("CPU").toStdWString();
}

std::wstring ResultsOverlay::timeText(const Race::Result & result) const
{
    if (result.finished)
    {
        return Timing::msecsToString(static_cast<int>(result.raceTime.count()));
    }

    const size_t currentLap { std::min(result.lapsCompleted + 1, m_race->lapCount()) };
    return QObject::tr("Lap %1/%2").arg(currentLap).arg(m_race->lapCount()).toStdWString();
}

std::wstring ResultsOverlay::gapText(const Race::Result & result, const Race::Result & winner) const
{
    if (!result.finished || !winner.finished || result.carIndex == winner.carIndex)
    {
        return L"";
    }

    const auto gap { result.raceTime - winner.raceTime };
    return L"+" + QString::number(static_cast<double>(gap.count()) / 1000.0, 'f', 2).toStdWString();
}

void ResultsOverlay::renderTitle(float centerX, float & y, float lineHeight)
{
    const QString title { Game::instance().mode() == Game::Mode::TimeTrial ? QObject::tr("Time Trial results") : QObject::tr("Race results") };
    renderText(title.toUpper().toStdWString(), centerX, y, lineHeight * glyphScale * 1.5f, titleColor, Align::Center);
    y -= lineHeight * 1.25f;

    const QString subtitle { QObject::tr("%1 · Laps: %2").arg(m_race->track().trackData().name()).arg(m_race->lapCount()) };
    renderText(subtitle.toUpper().toStdWString(), centerX, y, lineHeight * glyphScale, headerColor, Align::Center);
    y -= lineHeight * 1.5f;
}

void ResultsOverlay::renderHeader(float x, float panelWidth, float & y, float lineHeight)
{
    const float size { lineHeight * glyphScale * 0.8f };
    renderText(QObject::tr("Pos").toUpper().toStdWString(), x + panelWidth * positionColumn, y, size, headerColor);
    renderText(L"#", x + panelWidth * numberColumn, y, size, headerColor);
    renderText(QObject::tr("Driver").toUpper().toStdWString(), x + panelWidth * driverColumn, y, size, headerColor);
    renderText(QObject::tr("Time").toUpper().toStdWString(), x + panelWidth * timeColumn, y, size, headerColor);
    renderText(QObject::tr("Gap").toUpper().toStdWString(), x + panelWidth * gapColumn, y, size, headerColor);
    renderText(QObject::tr("Best lap").toUpper().toStdWString(), x + panelWidth * bestLapColumn, y, size, headerColor);
    y -= lineHeight;
}

void ResultsOverlay::renderRows(const std::vector<Race::Result> & results, float x, float panelWidth, float & y, float lineHeight)
{
    const float size { lineHeight * glyphScale };
    for (auto && result : results)
    {
        if (result.isHuman)
        {
            const float margin { panelWidth * 0.02f };
            renderPanel(x + margin, y - lineHeight * 0.3f, panelWidth - 2 * margin, lineHeight, humanRowColor);
        }

        const MCGLColor & color { result.finished ? finishedColor : racingColor };
        const std::wstring position { result.position < m_positionTexts.size() ? m_positionTexts.at(result.position) : std::to_wstring(result.position) };
        renderText(position, x + panelWidth * positionColumn, y, size, color);
        renderText(std::to_wstring(result.carIndex + 1), x + panelWidth * numberColumn, y, size, color);
        renderText(driverName(result), x + panelWidth * driverColumn, y, size, color);
        renderText(timeText(result), x + panelWidth * timeColumn, y, size, color);
        renderText(gapText(result, results.front()), x + panelWidth * gapColumn, y, size, color);
        renderText(Timing::msecsToString(result.bestLapTime), x + panelWidth * bestLapColumn, y, size, color);
        y -= lineHeight;
    }
}

void ResultsOverlay::renderAchievements(float centerX, float & y, float lineHeight)
{
    std::vector<QString> texts;
    const auto & achievements { m_race->achievements() };
    if (achievements.raceRecord)
    {
        texts.push_back(QObject::tr("New race record!"));
    }
    if (achievements.lapRecord)
    {
        texts.push_back(QObject::tr("New lap record!"));
    }
    if (achievements.bestPosition)
    {
        texts.push_back(QObject::tr("A new best pos!"));
    }
    if (achievements.trackUnlocked)
    {
        texts.push_back(QObject::tr("A new track unlocked!"));
    }

    if (!texts.empty())
    {
        y -= lineHeight * 0.5f;
        for (auto && text : texts)
        {
            renderText(text.toUpper().toStdWString(), centerX, y, lineHeight * glyphScale, achievementColor, Align::Center);
            y -= lineHeight;
        }
    }
}

void ResultsOverlay::render()
{
    if (!m_race)
    {
        return;
    }

    glDisable(GL_DEPTH_TEST);

    const float lineHeight { static_cast<float>(height()) / linesPerScreen };
    const float panelWidth { std::min(static_cast<float>(width()) * 0.9f, lineHeight * 30) };
    const float panelHeight { static_cast<float>(height()) - lineHeight };
    const float panelX { (static_cast<float>(width()) - panelWidth) / 2 };
    const float panelY { lineHeight / 2 };
    const float centerX { static_cast<float>(width()) / 2 };

    renderPanel(panelX, panelY, panelWidth, panelHeight, panelColor);

    float y { panelY + panelHeight - lineHeight * 1.5f };
    renderTitle(centerX, y, lineHeight);
    renderHeader(panelX, panelWidth, y, lineHeight);
    renderRows(m_race->results(), panelX, panelWidth, y, lineHeight);
    renderAchievements(centerX, y, lineHeight);

    const QString footer { QObject::tr("Enter: Continue") };
    renderText(footer.toUpper().toStdWString(), centerX, panelY + lineHeight * 0.5f, lineHeight * glyphScale * 0.8f, headerColor, Align::Center);
}

void ResultsOverlay::renderWaitingCard(size_t player, int width, int height)
{
    if (!m_race || player >= m_playerNames.size())
    {
        return;
    }

    const auto results { m_race->results() };
    const auto result { std::find_if(results.begin(), results.end(), [player](auto && result) { return result.carIndex == player; }) };
    if (result == results.end() || !result->finished)
    {
        return;
    }

    glDisable(GL_DEPTH_TEST);

    const float lineHeight { static_cast<float>(height) / 12 };
    const float cardWidth { std::min(static_cast<float>(width) * 0.9f, lineHeight * 14) };
    const float cardHeight { lineHeight * 4 };
    const float cardX { (static_cast<float>(width) - cardWidth) / 2 };
    const float cardY { (static_cast<float>(height) - cardHeight) / 2 };
    const float centerX { static_cast<float>(width) / 2 };

    renderPanel(cardX, cardY, cardWidth, cardHeight, panelColor);

    float y { cardY + cardHeight - lineHeight * 1.25f };
    const std::wstring position { result->position < m_positionTexts.size() ? m_positionTexts.at(result->position) : std::to_wstring(result->position) };
    const QString title { QObject::tr("Finished %1").arg(QString::fromStdWString(position)) };
    renderText(title.toUpper().toStdWString(), centerX, y, lineHeight * glyphScale * 1.2f, titleColor, Align::Center);
    y -= lineHeight;

    renderText(Timing::msecsToString(static_cast<int>(result->raceTime.count())), centerX, y, lineHeight * glyphScale, finishedColor, Align::Center);
    y -= lineHeight;

    const QString & otherPlayer { m_playerNames.at(1 - player) };
    renderText(QObject::tr("Waiting for %1..").arg(otherPlayer).toStdWString(), centerX, y, lineHeight * glyphScale * 0.8f, headerColor, Align::Center);
}
