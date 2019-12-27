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

#ifndef MCMESHVIEW_HH
#define MCMESHVIEW_HH

#include "mcmesh.hh"
#include "mcshapeview.hh"

/*! \class MCMeshView.
 *  \brief View class for MCShape. Renders the given MCMesh.
 */
class MCMeshView : public MCShapeView
{
public:
    /*! Constructor.
     * \param typeId See MCShapeView::MCShapeView().
     * \param mesh mesh to be used. */
    explicit MCMeshView(const std::string & viewID, MCMeshPtr mesh);

    //! Destructor
    virtual ~MCMeshView() override;

    //! Set mesh
    void setMesh(MCMeshPtr mesh);

    //! Get mesh or nullptr if not set.
    MCMeshPtr mesh() const;

    //! \reimp
    void setShaderProgram(MCGLShaderProgramPtr program) override;

    //! \reimp
    void setShadowShaderProgram(MCGLShaderProgramPtr program) override;

    //! \reimp
    virtual void render(const MCVector3dF & location, float angle, MCCamera * camera = nullptr) override;

    //! \reimp
    virtual void renderShadow(const MCVector3dF & location, float angle, MCCamera * camera = nullptr) override;

    //! \reimp
    virtual const MCBBoxF & bbox() const override;

    //! \reimp
    virtual void bind() override;

    //! \reimp
    virtual void bindShadow() override;

    //! \reimp
    virtual void release() override;

    //! \reimp
    virtual void releaseShadow() override;

    //! \reimp
    virtual void setScale(const MCVector3dF & scale) override;

    //! \reimp
    virtual MCGLObjectBase * object() const override;

private:
    DISABLE_COPY(MCMeshView);
    DISABLE_ASSI(MCMeshView);

    void updateBBox();

    MCMeshPtr m_mesh;

    MCBBoxF m_bbox;
};

#endif // MCMESHVIEW_HH
