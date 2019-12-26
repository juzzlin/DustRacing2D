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

#ifndef TIMINGOVERLAY_HPP
#define TIMINGOVERLAY_HPP

#include "carstatusview.hpp"
#include "overlaybase.hpp"

#include <QObject>

#include <memory>
#include <string>
#include <vector>

#include <MCTextureText>

class Car;
class MCTextureFontManager;
class MCTextureFont;
class Race;
class Timing;

//! Renders the main information overlay on top of the game scene.
class TimingOverlay : public QObject, public OverlayBase
{
    Q_OBJECT

public:
    //! Constructor.
    TimingOverlay();

    //! \reimp
    virtual void render() override;

    //! \reimp
    virtual bool update() override;

    //! Show timing for the given car.
    void setCarToFollow(const Car & car);

    //! Set current race.
    void setRace(std::shared_ptr<Race> race);

public slots:

    //! Blink car status view if followed car matches the argument.
    void blinkCarStatus(const Car &);

private slots:

    void setLapRecord(int msecs);

    void setRaceRecord(int msecs);

    void blinkLapRecord();

    void blinkRaceRecord();

    void blinkCarStatus();

private:
    void renderCarStatusView();

    void renderCurrentLap();

    void renderCurrentLapTime();

    void renderLastLapTime();

    void renderPosition();

    void renderRaceTime();

    void renderRecordLapTime();

    void renderSpeed();

    MCTextureFontManager & m_fontManager;

    MCTextureFont & m_font;

    MCTextureText m_text;

    const Car * m_car;

    std::shared_ptr<Race> m_race;

    std::vector<std::wstring> m_posTexts;

    bool m_showLapRecordTime;

    bool m_showRaceTime;

    bool m_showCarStatus;

    CarStatusView m_carStatusView;
};

#endif // TIMINGOVERLAY_HPP
