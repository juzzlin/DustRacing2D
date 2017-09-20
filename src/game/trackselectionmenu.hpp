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

#ifndef TRACKSELECTIONMENU_HPP
#define TRACKSELECTIONMENU_HPP

#include "surfacemenu.hpp"

class Track;
class TrackItem;
class Scene;
class MCSurface;

//! Allows the user to select a track to play.
class TrackSelectionMenu : public SurfaceMenu
{
public:

    //! Constructor.
    TrackSelectionMenu(std::string id, int width, int height, Scene & scene);

    //! Add a track to the list of selectable tracks.
    void addTrack(Track & track);

    //! Returns the selected track or nullptr.
    Track * selectedTrack() const;

    //! \reimp
    virtual void left() override;

    //! \reimp
    virtual void right() override;

    //! \reimp
    virtual void up() override;

    //! \reimp
    virtual void down() override;

    //! \reimp
    virtual void selectCurrentItem() override;

private:

    Track * m_selectedTrack;
    Scene & m_scene;
};

#endif // TRACKSELECTIONMENU_HPP
