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

#ifndef TRACKPREVIEWOVERLAY_HPP
#define TRACKPREVIEWOVERLAY_HPP

#include "overlaybase.hpp"

class Track;

//! Renders a preview of the given track.
class TrackPreviewOverlay : public OverlayBase
{
public:

    //! Constructor.
    TrackPreviewOverlay();

    //! Set the track to be rendered or nullptr.
    void setTrack(Track * track);

    //! \reimp
    virtual void render();

private:

    Track * m_track;
};

#endif // TRACKPREVIEWOVERLAY_HPP
