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

#ifndef STARTLIGHTS_H
#define STARTLIGHTS_H

#include "overlaybase.h"

class MCSurface;
class MCTextureFontManager;

//! Renders startlights on top of the game scene.
class Startlights : public OverlayBase
{
public:

    //! Constructor.
    Startlights();

    //! \reimp
    virtual void render();

private:

//    MCSurface & m_body;
//    MCSurface & m_hand;
//    MCSurface & m_center;
    MCUint m_x, m_y;
};

#endif // STARTLIGHTS_H
