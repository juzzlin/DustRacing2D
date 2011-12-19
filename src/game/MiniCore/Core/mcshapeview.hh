// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCSHAPEVIEW_HH
#define MCSHAPEVIEW_HH

#include "mctypes.hh"
#include "mcmacros.hh"
#include "mcvector3d.hh"

class MCCamera;

/*! \class MCShapeView.
 *  \brief MCShapeView is an abstract base class (2d) for view objects.
 *
 * MCShape-derived objects uses MCShapeView to actually draw themselves.
 */
class MCShapeView
{
public:

    //! Constructor
    MCShapeView() {};

    //! Destructor
    virtual ~MCShapeView() {};

    /*! Render the shape at the current location using given camera window.
     * \param l Location.
     * \param angle Angle in degrees about the Z-axis.
     * \param p Camera window. If nullptr, then no translation or clipping done.
     */
    virtual void render(
        const MCVector3d<MCFloat> & l, MCUint angle, MCCamera * p = nullptr) = 0;

    /*! Render shadow for the shape at the current location using given camera window.
     * \param l Location.
     * \param angle Angle in degrees about the Z-axis.
     * \param p Camera window. If nullptr, then no translation or clipping done.
     */
    virtual void renderShadow(
        const MCVector3d<MCFloat> & l, MCUint angle, MCCamera * p = nullptr) = 0;

    /*! Render the scaled shape at the current location using given camera window.
     * \param l Location.
     * \param angle Angle in degrees about the Z-axis.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     * \param p Camera window. If nullptr, then no translation or clipping done.
     */
    virtual void renderScaled(
        const MCVector3d<MCFloat> & l, MCUint angle,
        MCFloat wr, MCFloat hr, MCCamera * p = nullptr) = 0;

    /*! Render scaled shadow for the shape at the current location using given camera window.
     * \param l Location.
     * \param angle Angle in degrees about the Z-axis.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     * \param p Camera window. If nullptr, then no translation or clipping done.
     */
    virtual void renderShadowScaled(
        const MCVector3d<MCFloat> & l, MCUint angle,
        MCFloat wr, MCFloat hr, MCCamera * p = nullptr) = 0;

private:

    //! Disable copy constructor and assignment
    DISABLE_ASSI(MCShapeView);
    DISABLE_COPY(MCShapeView);
};

#endif // MCSHAPEVIEW_HH
