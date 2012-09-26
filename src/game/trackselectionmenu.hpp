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

#ifndef TRACKSELECTIONMENU_HPP
#define TRACKSELECTIONMENU_HPP

#include "surfacemenu.hpp"

class Track;
class TrackItem;
class Scene;
class StateMachine;
class MCSurface;

//! Allows the user to select a track to play.
class TrackSelectionMenu : public SurfaceMenu
{
public:

    //! Constructor.
    TrackSelectionMenu(std::string id,
        int width, int height, Scene & scene, StateMachine & sm);

    //! Add a track to the list of selectable tracks.
    void addTrack(Track & track);

    //! Returns the selected track or nullptr.
    Track * selectedTrack() const;

    //! \reimp
    virtual void left();

    //! \reimp
    virtual void right();

    //! \reimp
    virtual void up();

    //! \reimp
    virtual void down();

    //! \reimp
    virtual void selectCurrentItem();

private:

    Track        * m_selectedTrack;
    Scene        & m_scene;
    StateMachine & m_sm;
};

#endif // TRACKSELECTIONMENU_HPP
