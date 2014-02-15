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
class MCCamera;
class MCGLShaderProgram;

/*! \class MCSurfaceView.
 *  \brief View class for MCShape. Renders the given MCSurface.
 */
class MCSurfaceView : public MCShapeView
{
public:

    /*! Constructor.
     * \param typeId See MCShapeView::MCShapeView().
     * \param surface Surface to be used. Ownership of surface is not changed. */
    explicit MCSurfaceView(
        const std::string & viewID,
        MCSurface * surface = nullptr);

    //! Destructor
    virtual ~MCSurfaceView();

    //! Set surface
    void setSurface(MCSurface & surface);

    //! Get surface or nullptr if not set.
    MCSurface * surface() const;

    //! \reimp
    void setShaderProgram(MCGLShaderProgramPtr program);

    //! \reimp
    void setShadowShaderProgram(MCGLShaderProgramPtr program);

    //! \reimp
    virtual void render(
        const MCVector3d<MCFloat> & l,
        MCFloat angle,
        MCCamera * p = nullptr);

    //! \reimp
    virtual void renderShadow(
        const MCVector3d<MCFloat> & l,
        MCFloat angle,
        MCCamera * p = nullptr);

    //! \reimp
    virtual void renderScaled(
        const MCVector3d<MCFloat> & l,
        MCFloat angle,
        MCFloat w,
        MCFloat h,
        MCCamera * p = nullptr);

    //! \reimp
    virtual void renderShadowScaled(
        const MCVector3d<MCFloat> & l, MCFloat angle,
        MCFloat w, MCFloat h, MCCamera * p = nullptr);

    //! \reimp
    virtual MCBBox<MCFloat> bbox() const;

    //! \reimp
    virtual void beginBatch();

    //! \reimp
    virtual void beginShadowBatch();

    //! \reimp
    virtual void endBatch();

    //! \reimp
    virtual void endShadowBatch();

private:

    DISABLE_COPY(MCSurfaceView);
    DISABLE_ASSI(MCSurfaceView);

    MCSurface * m_surface;
};

#endif // MCSURFACEVIEW_HH
