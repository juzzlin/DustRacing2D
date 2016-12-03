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

#ifndef MCSURFACEOBJECTRENDERERLEGACY_HH
#define MCSURFACEOBJECTRENDERERLEGACY_HH

#include <MCGLEW>

#include "mcmacros.hh"
#include "mcobjectrendererbase.hh"
#include "mcworldrenderer.hh"

#include <vector>

class MCSurfaceObject;
class MCCamera;
class MCObject;

class MCSurfaceObjectRendererLegacy : public MCObjectRendererBase
{
public:

    explicit MCSurfaceObjectRendererLegacy(int maxBatchSize = 1024);

    //! Destructor.
    virtual ~MCSurfaceObjectRendererLegacy();

private:

    DISABLE_COPY(MCSurfaceObjectRendererLegacy);
    DISABLE_ASSI(MCSurfaceObjectRendererLegacy);

    /*! Populate the current batch.
     *  \param Objects The vector of Object data to be rendered.
     *  \param camera The camera window. */
    void setBatch(ObjectVector & Objects, MCCamera * camera = nullptr, bool isShadow = false) override;

    //! Render the current Object batch.
    void render() override;

    //! Render the current Object batch as shadows.
    void renderShadows() override;

    //! \reimp
    void setAttributePointers() override;

    MCGLVertex * m_vertices;

    MCGLVertex * m_normals;

    MCGLTexCoord * m_texCoords;

    MCGLColor * m_colors;

    friend class MCWorldRenderer;
};

#endif // MCSURFACEOBJECTRENDERERLEGACY_HH
