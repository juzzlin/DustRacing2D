// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MCGLSCENE_HH
#define MCGLSCENE_HH

#include "mctypes.hh"

//! Basic initializations of a OpenGL scene suitable for
//! 2D/2.5D games.
class MCGLScene
{
public:

    //! Constructor.
    MCGLScene();

    //! Destructor.
    virtual ~MCGLScene();

    //! Initialize. Re-implement if desired.
    virtual void initialize();

    //! Resize / set projection matrices. Re-implement if desired.
    virtual void resize(MCUint viewWidth, MCUint viewHeight,
        MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle);

    //! Set viewer's position. Automatically called by resize().
    virtual void setViewerPosition(MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle);
};

#endif // MCGLSCENE_HH
