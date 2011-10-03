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

/*! MCSurface object that stores an OpenGL texture handle.
 *  A MCSurface can also be rendered straightly as a standalone object
 *  without creating any vertices, like a surface. This is useful in 2D-engines.
 */
class MCSurface
{
public:

    //! Constructor.
    //! \param handle Handle of the corresponding OpenGL texture.
    //! \param width  Desired width of the surface when rendered 1:1.
    //! \param height Desired height of the surface when rendered 1:1.
    //! \param useAlphaTest Alpha is tested for colorkeying if true. Pixels whose
    //!        alpha is < 0.5f are not rendered.
    MCSurface(GLuint handle, MCFloat width, MCFloat height, bool useAlphaTest);

    //! Constructor.
    //! \param handle Handle of the corresponding OpenGL texture.
    //! \param width  Desired width of the surface when rendered 1:1.
    //! \param height Desired height of the surface when rendered 1:1.
    //! \param center Vector that defines a custom rotation point other than
    //!        (width / 2, height / 2).
    //! \param useAlphaTest Alpha is tested for colorkeying if true. Pixels whose
    //!        alpha is < 0.5f are not rendered.
    MCSurface(GLuint handle, MCFloat width, MCFloat height, const MCVector2d<MCFloat> & center, bool useAlphaTest);

    //! Destructor.
    virtual ~MCSurface();

    //! Set role string.
    void setRole(const string & role);

    //! Get role string.
    const string & role() const;

    //! Set category string.
    void setCategory(const string & category);

    //! Get category string.
    const string & category() const;

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
