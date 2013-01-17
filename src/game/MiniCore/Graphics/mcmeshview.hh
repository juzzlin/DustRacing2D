// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcshapeview.hh"
#include "mcmesh.hh"

/*! \class MCMeshView.
 *  \brief View class for MCShape. Renders the given MCMesh.
 */
class MCMeshView : public MCShapeView
{
public:
    /*! Constructor.
     * \param typeId See MCShapeView::MCShapeView().
     * \param mesh mesh to be used. Ownership of mesh is not changed. */
    explicit MCMeshView(
        const std::string & viewID,
        MCMesh * mesh = nullptr,
        bool batchMode = false);

    //! Destructor
    virtual ~MCMeshView();

    //! Set mesh
    void setMesh(MCMesh & mesh);

    //! Get mesh or nullptr if not set.
    MCMesh * mesh() const;

    //! \reimp
    void setShaderProgram(MCGLShaderProgram * program);

    //! \reimp
    void setShadowShaderProgram(MCGLShaderProgram * program);

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
    virtual void endBatch();

    //! \reimp
    virtual void beginShadowBatch();

    //! \reimp
    virtual void endShadowBatch();

private:

    DISABLE_COPY(MCMeshView);
    DISABLE_ASSI(MCMeshView);

    MCMesh * m_mesh;
    bool m_batchMode;
};

#endif // MCMESHVIEW_HH
