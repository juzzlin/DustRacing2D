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

#ifndef MCSURFACEVIEW_HH
#define MCSURFACEVIEW_HH

#include "mcshapeview.hh"

class MCSurface;
class MCSurfaceViewImpl;
class MCCamera;

/*! \class MCSurfaceView.
 *  \brief View class for MCShape. Renders the given MCSurface.
 */
class MCSurfaceView : public MCShapeView
{
public:

    /*! Constructor.
     * \param pSurface Surface to be used. Ownership of pSurface is not changed.
     */
    explicit MCSurfaceView(MCSurface * pSurface = nullptr);

    //! Destructor
    virtual ~MCSurfaceView();

    //! Set surface
    void setSurface(MCSurface * p);

    //! Get surface
    MCSurface * surface() const;

    //! \reimp
    virtual void render(const MCVector3d<MCFloat> & l, MCUint angle,
        MCCamera * p = nullptr);

    //! \reimp
    virtual void renderShadow(const MCVector3d<MCFloat> & l, MCUint angle,
        MCCamera * p = nullptr);

    //! \reimp
    virtual void renderScaled(const MCVector3d<MCFloat> & l, MCUint angle,
        MCFloat wr, MCFloat hr, MCCamera * p = nullptr);

    //! \reimp
    virtual void renderShadowScaled(const MCVector3d<MCFloat> & l, MCUint angle,
        MCFloat wr, MCFloat hr, MCCamera * p = nullptr);

private:

    DISABLE_COPY(MCSurfaceView);
    DISABLE_ASSI(MCSurfaceView);
    MCSurfaceViewImpl * const m_pImpl;
};

#endif // MCSURFACEVIEW_HH
