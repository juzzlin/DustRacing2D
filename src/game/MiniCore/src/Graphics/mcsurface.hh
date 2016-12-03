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

#include "mctypes.hh"
#include "mcmacros.hh"
#include "mcbbox.hh"
#include "mcglcolor.hh"
#include "mcglobjectbase.hh"
#include "mcglmaterial.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <cmath>
#include <string>

class  MCCamera;
class  MCGLShaderProgram;
struct MCGLTexCoord;
class  MCGLVertex;

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
    MCSurface(
        std::string handle,
        MCGLMaterialPtr material,
        MCFloat width,
        MCFloat height,
        MCFloat z0 = 0,
        MCFloat z1 = 0,
        MCFloat z2 = 0,
        MCFloat z3 = 0);

    /*! Constructor.
     *  \param handle Handle (or name) of the surface.
     *  \param width  Desired width of the surface when rendered 1:1.
     *  \param height Desired height of the surface when rendered 1:1.
     *  \param z Z-coordinate common for all vertices. */
    MCSurface(
        std::string handle,
        MCGLMaterialPtr material,
        MCFloat width,
        MCFloat height,
        MCFloat z);

    /*! Constructor.
     *  \param handle Handle (or name) of the surface.
     *  \param width  Desired width of the surface when rendered 1:1.
     *  \param height Desired height of the surface when rendered 1:1.
     *  \param texCoords Array including texture coordinates of the four vertices. */
    MCSurface(
        std::string handle,
        MCGLMaterialPtr material,
        MCFloat width,
        MCFloat height,
        const MCGLTexCoord texCoords[4]);

    //! Destructor.
    virtual ~MCSurface() {};

    const std::string & handle() const;

    /*! Enable / disable alpha blend.
     *  \param useAlphaBlend Alpha blending is enabled if true.
     *  \param src Source alpha function used in the alpha blending. Has no effect
     *         if useAlphaBlend equals false.
     *  \param dst Destination alpha function used in the alpha blending. Has no effect
     *         if useAlphaBlend equals false. */
    void setAlphaBlend(bool useAlphaBlend, GLenum src = GL_SRC_ALPHA, GLenum dst = GL_ONE_MINUS_SRC_ALPHA);

    /*! Runs the corresponding GL-commands defined in setAlphaBlend().
     *  This is done automatically, but doAlphaBlend() can be used if
     *  someone else renders the surface by using the texture
     *  handle and wants to run the configured alpha blending. */
    void doAlphaBlend();

    //! Update texture coordinates.
    void updateTexCoords(const MCGLTexCoord texCoords[4]);

    //! Set color.
    void setColor(const MCGLColor & color);

    //! Set scaling factors.
    void setScale(MCFloat x, MCFloat y, MCFloat z);

    //! Set texture size.
    void setSize(MCFloat w, MCFloat h);

    /*! Render by using the default size.
     * \param pos The position.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height. */
    void render(MCCamera * camera, MCVector3dFR pos, MCFloat angle, bool autoBind = true);

    //! Render (fake) shadow
    void renderShadow(MCCamera * camera, MCVector3dFR pos, MCFloat angle, bool autoBind = true);

    //! Render the vertex buffer only. bind() must be called separately.
    void render();

    //! Get width
    MCFloat width() const;

    //! Get height
    MCFloat height() const;

    //! Get minimum Z
    MCFloat minZ() const;

    //! Get maximum Z
    MCFloat maxZ() const;

    //! \reimp
    virtual void bind() override;

    //! \reimp
    virtual void bindShadow() override;

    //! \reimp
    virtual void release() override;

    //! \reimp
    virtual void releaseShadow() override;

private:

    DISABLE_COPY(MCSurface);
    DISABLE_ASSI(MCSurface);

    void init(std::string handle, MCGLMaterialPtr material, MCFloat width, MCFloat height);

    void initVBOs();

    std::string m_handle;

    MCFloat m_w;

    MCFloat m_w2;

    MCFloat m_h;

    MCFloat m_h2;

    MCFloat m_minZ;

    MCFloat m_maxZ;

    bool m_useAlphaBlend;

    GLenum m_src;

    GLenum m_dst;

    MCGLColor m_color;

    MCFloat m_sx, m_sy, m_sz;
};

#endif // MCSURFACE_HH
