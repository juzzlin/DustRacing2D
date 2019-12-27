// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
     * \param handle See MCShapeView::MCShapeView().
     * \param surface Surface to be used. */
    explicit MCSurfaceView(const std::string & handle, std::shared_ptr<MCSurface> surface);

    //! Destructor
    virtual ~MCSurfaceView() override;

    //! Sets surface and updates the view handle with the handle of the surface.
    void setSurface(std::shared_ptr<MCSurface> surface);

    //! Get surface or nullptr if not set.
    std::shared_ptr<MCSurface> surface() const;

    //! \reimp
    void setShaderProgram(MCGLShaderProgramPtr program) override;

    //! \reimp
    void setShadowShaderProgram(MCGLShaderProgramPtr program) override;

    //! \reimp
    virtual void render(
      const MCVector3dF & l,
      float angle,
      MCCamera * p = nullptr) override;

    //! \reimp
    virtual void renderShadow(
      const MCVector3dF & l,
      float angle,
      MCCamera * p = nullptr) override;

    //! \reimp
    virtual void bind() override;

    //! \reimp
    virtual void bindShadow() override;

    //! \reimp
    virtual void release() override;

    //! \reimp
    virtual void releaseShadow() override;

    //! \reimp
    virtual const MCBBoxF & bbox() const override;

    //! \reimp
    virtual void setColor(const MCGLColor & color) override;

    //! \reimp
    virtual void setScale(const MCVector3dF & scale) override;

    //! \reimp
    virtual MCGLObjectBase * object() const override;

private:
    DISABLE_COPY(MCSurfaceView);
    DISABLE_ASSI(MCSurfaceView);
    DISABLE_MOVE(MCSurfaceView);

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // MCSURFACEVIEW_HH
