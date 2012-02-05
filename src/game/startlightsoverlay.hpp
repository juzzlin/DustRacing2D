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

#ifndef STARTLIGHTSOVERLAY_HPP
#define STARTLIGHTSOVERLAY_HPP

#include "overlaybase.hpp"
#include "MiniCore/Core/MCTypes"

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
    virtual void render();

    //! \reimp
    virtual void setDimensions(MCUint width, MCUint height);

private:

    void renderLights(MCUint rows, MCUint litRows) const;

    MCSurface & m_startLightOn;
    MCSurface & m_startLightOff;
    Startlights & m_model;
};

#endif // STARTLIGHTSOVERLAY_HPP
