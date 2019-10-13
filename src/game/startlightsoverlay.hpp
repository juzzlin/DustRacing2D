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

#ifndef STARTLIGHTSOVERLAY_HPP
#define STARTLIGHTSOVERLAY_HPP

#include "overlaybase.hpp"

class MCSurface;
class MCTextureFontManager;

class Startlights;

//! Renders startlights on top of the game scene.
class StartlightsOverlay : public OverlayBase
{
public:
    //! Constructor.
    StartlightsOverlay(Startlights & model);

    //! \reimp
    virtual void render() override;

    //! \reimp
    virtual void setDimensions(int width, int height) override;

private:
    void renderLights(int rows, int litRows, float glowScale, bool glowAlways = false) const;

    MCSurface & m_startLightOn;
    MCSurface & m_startLightOnCorner;
    MCSurface & m_startLightOff;
    MCSurface & m_startLightOffCorner;
    MCSurface & m_startLightGlow;
    Startlights & m_model;
    float m_alpha;
};

#endif // STARTLIGHTSOVERLAY_HPP
