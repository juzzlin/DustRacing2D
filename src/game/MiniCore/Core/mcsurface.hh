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

#ifndef MCSURFACE_HH
#define MCSURFACE_HH

#include "mcmacros.hh"
#include "mctypes.hh"
#include "mcbbox.hh"
#include "mcvector2d.hh"

#include <cmath>
#include <string>
#include <GL/gl.h>

using std::string;

class MCCamera;
class MCSurfaceImpl;

/*! MCSurface is a renderable object bound to an OpenGL texture handle.
 *  MCSurface can be rendered straightly as a standalone object
 *  without creating any vertices, like a surface. This is useful in 2D-engines.
 */
class MCSurface
{
public:

    //! Constructor.
    //! \param handle Handle of the corresponding OpenGL texture.
    //! \param width  Desired width of the surface when rendered 1:1.
    //! \param height Desired height of the surface when rendered 1:1.
    MCSurface(GLuint handle, MCFloat width, MCFloat height);

    //! Destructor.
    virtual ~MCSurface();

    //! Set the point the surface is centered about.
    //! \param center Vector that defines a custom rotation point other than
    //!        (width / 2, height / 2) which is the default.
    void setCenter(const MCVector2d<MCFloat> & center);

    //! Enable / disable alpha test.
    //! \param useAlphaTest Alpha is tested for colorkeying if true.
    //! \param alphaFunc Alpha function used in alpha test. Has no effect
    //!        if useAlphaTest equals false.
    //! \param threshold Threshold used in the alpha test. Has no effect
    //!        if useAlphaTest equals false.
    void setAlphaTest(bool useAlphaTest, GLenum alphaFunc = GL_GREATER, GLclampf threshold = 0.5f);

   /*! Get bounding box for a rotated surface
    * \param x x-coordinate
    * \param y y-coordinate
    * \param angle Rotation angle (0..360)
    */
    MCBBox<MCFloat> rotatedBBox(MCFloat x, MCFloat y, int angle);

   /*! Get bounding box for a rotated and scaled surface
    * \param x x-coordinate
    * \param y y-coordinate
    * \param angle Rotation angle (0..360)
    * \param w2 half of the scaled width
    * \param h2 half of the scaled height
    */
    MCBBox<MCFloat> rotatedScaledBBox(MCFloat x, MCFloat y, int angle, MCFloat w2, MCFloat h2);

    //! Render using default size + z
    void render(MCCamera * pCamera, MCFloat x, MCFloat y, MCFloat z, int angle);

   /*! Render scaled.
    * \param x x-coordinate
    * \param y y-coordinate
    * \param z z-coordinate
    * \param wr Half of the wanted width.
    * \param hr Half of the wanted height.
    */
    void renderScaled(MCCamera * pCamera, MCFloat x, MCFloat y, MCFloat z, MCFloat wr, MCFloat hr, int angle);

    //! Render (fake) shadow
    void renderShadow(MCCamera * pCamera, MCFloat x, MCFloat y, int angle);

   /*! Render (fake) shadow scaled.
    * \param x x-coordinate
    * \param y y-coordinate
    * \param z z-coordinate
    * \param wr Half of the wanted width.
    * \param hr Half of the wanted height.
    */
    void renderShadowScaled(MCCamera * pCamera, MCFloat x, MCFloat y, MCFloat wr, MCFloat hr, int angle);

    //! Get OpenGL texture handle
    GLuint handle() const;

    //! Get width
    MCFloat width() const;

    //! Get height
    MCFloat height() const;

    //! Get center
    MCVector2d<MCFloat> center() const;

private:

    DISABLE_COPY(MCSurface);
    DISABLE_ASSI(MCSurface);
    MCSurfaceImpl * const m_pImpl;
};

#endif // MCSURFACE_HH
