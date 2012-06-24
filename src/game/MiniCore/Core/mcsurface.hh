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

#include "mctypes.hh"
#include "mcmacros.hh"
#include "mcbbox.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <cmath>
#include <string>

#include <GL/gl.h>

using std::string;

class MCCamera;
class MCGLTexCoord;
class MCGLVertex;
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

    //! Constructor.
    //! \param handle Handle of the corresponding OpenGL texture.
    //! \param width  Desired width of the surface when rendered 1:1.
    //! \param height Desired height of the surface when rendered 1:1.
    //! \param texCoords Array including texture coordinates of the four vertices.
    MCSurface(GLuint handle, MCFloat width, MCFloat height, const MCGLTexCoord texCoords[4]);

    //! Destructor.
    virtual ~MCSurface();

    //! Set the point the surface is centered about.
    //! \param center Vector that defines a custom rotation point other than
    //!        (width / 2, height / 2) which is the default.
    void setCenter(MCVector2dFR center);

    //! Enable / disable alpha test.
    //! \param useAlphaTest Alpha is tested for colorkeying if true.
    //! \param alphaFunc Alpha function used in alpha test. Has no effect
    //!        if useAlphaTest equals false.
    //! \param threshold Threshold used in the alpha test. Has no effect
    //!        if useAlphaTest equals false.
    void setAlphaTest(
        bool useAlphaTest, GLenum alphaFunc = GL_GREATER, GLclampf threshold = 0.5);

    //! Runs the corresponding GL-commands defined in setAlphaTest().
    //! This is done automatically, but doAlphaTest() can be used if
    //! someone else renders the surface by using the texture
    //! handle and wants to run the configured alpha test.
    void doAlphaTest() const;

    //! Enable / disable alpha blend.
    //! \param useAlphaBlend Alpha blending is enabled if true.
    //! \param src Source alpha function used in the alpha blending. Has no effect
    //!        if useAlphaBlend equals false.
    //! \param dst Destination alpha function used in the alpha blending. Has no effect
    //!        if useAlphaBlend equals false.
    void setAlphaBlend(
        bool useAlphaBlend, GLenum src = GL_SRC_ALPHA, GLenum dst = GL_ONE_MINUS_SRC_ALPHA);

    //! Runs the corresponding GL-commands defined in setAlphaBlend().
    //! This is done automatically, but doAlphaBlend() can be used if
    //! someone else renders the surface by using the texture
    //! handle and wants to run the configured alpha blending.
    void doAlphaBlend() const;

    //! Set texture coordinates.
    void setTexCoords(const MCGLTexCoord texCoords[4]);

   /*! Get bounding box for a rotated surface
    * \param pos The position.
    * \param angle Rotation angle (0..360)
    */
    MCBBox<MCFloat> rotatedBBox(MCVector2dFR, MCFloat angle);

   /*! Get bounding box for a rotated and scaled surface
    * \param pos The position.
    * \param angle Rotation angle (0..360)
    * \param w2 half of the scaled width
    * \param h2 half of the scaled height
    */
    MCBBox<MCFloat> rotatedScaledBBox(MCVector2dFR pos, MCFloat angle, MCFloat w2, MCFloat h2);

    /*! Render by using the default size.
     * \param pos The position.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     */
    void render(MCCamera * pCamera, MCVector3dFR pos, MCFloat angle,
        bool autoClientState = true);

   /*! Render scaled.
    * \param pos The position.
    * \param wr Half of the wanted width.
    * \param hr Half of the wanted height.
    * \param autoClientState Automatically enable/disable OpenGL client states.
    */
    void renderScaled(
        MCCamera * pCamera, MCVector3dFR pos, MCFloat wr, MCFloat hr, MCFloat angle,
        bool autoClientState = true);

    //! Render (fake) shadow
    void renderShadow(MCCamera * pCamera, MCVector2dFR pos, MCFloat angle,
        bool autoClientState = true);

   /*! Render (fake) shadow scaled.
    * \param pos The position.
    * \param wr Half of the wanted width.
    * \param hr Half of the wanted height.
    */
    void renderShadowScaled(
        MCCamera * pCamera, MCVector2dFR pos, MCFloat wr, MCFloat hr, MCFloat angle,
        bool autoClientState = true);

    /*! Manually enable/disable OpenGL client states and texturing environment.
     *  This can be used to save some function calls when rendering the same
     *  surface multiple times.
     *  \see render()
     */
    void enableClientState(bool enable);

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

    void callList();

    void initVBOs(
        const MCGLVertex   * vertices,
        const MCGLVertex   * normals,
        const MCGLTexCoord * texCoords,
        const GLfloat      * colors);

    void renderVBOs(bool autoClientState = true);

    enum VBOType
    {
        VBOVertex,
        VBONormal,
        VBOTexture,
        VBOColor,
        VBOTypes
    };

    GLuint m_handle;
    MCFloat m_w;
    MCFloat m_w2;
    MCFloat m_h;
    MCFloat m_h2;
    MCVector2dF m_center;
    bool m_centerSet;
    bool m_useAlphaTest;
    GLenum m_alphaFunc;
    GLclampf m_alphaThreshold;
    bool m_useAlphaBlend;
    GLenum m_src;
    GLenum m_dst;
    GLuint m_vbos[VBOTypes];
};

#endif // MCSURFACE_HH
