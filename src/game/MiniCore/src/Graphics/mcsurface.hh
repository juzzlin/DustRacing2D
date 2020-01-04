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

#ifndef MCSURFACE_HH
#define MCSURFACE_HH

#include <MCGLEW>

#include "mcbbox.hh"
#include "mcglmaterial.hh"
#include "mcglobjectbase.hh"
#include "mcmacros.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <cmath>
#include <memory>
#include <string>

class MCCamera;
class MCGLShaderProgram;
struct MCGLTexCoord;
class MCGLVertex;
class QImage;

/*! MCSurface is a (2D) renderable object bound to an OpenGL texture handle.
 *  MCSurface can be rendered as a standalone object. Despite being a
 *  2D object, it's possible to assign Z-values to the vertices in order to
 *  easily create tilted surfaces. */
class MCSurface : public MCGLObjectBase
{
public:
    /*! Constructor.
     *  \param handle Handle (or name) of the surface.
     *  \param width  Desired width of the surface when rendered 1:1.
     *  \param height Desired height of the surface when rendered 1:1.
     *  \param z0 Z-coordinate for vertex[0]. Enables tilted surfaces.
     *  \param z1 Z-coordinate for vertex[1]. Enables tilted surfaces.
     *  \param z2 Z-coordinate for vertex[2]. Enables tilted surfaces.
     *  \param z3 Z-coordinate for vertex[3]. Enables tilted surfaces. */
    MCSurface(std::string handle, MCGLMaterialPtr material, float width, float height, float z0 = 0, float z1 = 0, float z2 = 0, float z3 = 0);

    /*! Constructor.
     *  \param handle Handle (or name) of the surface.
     *  \param width  Desired width of the surface when rendered 1:1.
     *  \param height Desired height of the surface when rendered 1:1.
     *  \param z Z-coordinate common for all vertices. */
    MCSurface(std::string handle, MCGLMaterialPtr material, float width, float height, float z);

    /*! Constructor.
     *  \param handle Handle (or name) of the surface.
     *  \param width  Desired width of the surface when rendered 1:1.
     *  \param height Desired height of the surface when rendered 1:1.
     *  \param texCoords Array including texture coordinates of the four vertices. */
    MCSurface(std::string handle, MCGLMaterialPtr material, float width, float height, const MCGLTexCoord texCoords[4]);

    //! Destructor.
    virtual ~MCSurface();

    //! Update texture coordinates.
    void updateTexCoords(const MCGLTexCoord texCoords[4]);

    void setAverageColor(const MCGLColor & color);

    const MCGLColor & averageColor() const;

private:
    DISABLE_COPY(MCSurface);
    DISABLE_ASSI(MCSurface);
    DISABLE_MOVE(MCSurface);

    void initVBOs();

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

using MCSurfacePtr = std::shared_ptr<MCSurface>;

#endif // MCSURFACE_HH
