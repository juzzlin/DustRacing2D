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

#ifndef OVERLAYBASE_H
#define OVERLAYBASE_H

#include "MiniCore/Core/MCBBox"
#include "MiniCore/Core/MCTypes"

class MCCamera;

//! Base class for overlays that are rendered on top of
//! the game scene.
class OverlayBase
{
public:

    //! Constructor.
    OverlayBase();

    //! Set overlay dimensions.
    void setDimensions(MCUint width, MCUint height);

    //! Get overlay width.
    MCUint width() const;

    //! Get overlay height.
    MCUint height() const;

    //! Destructor.
    virtual ~OverlayBase();

    //! Render.
    virtual void render() = 0;

private:

    MCUint m_width, m_height;
};

#endif // OVERLAYBASE_H
