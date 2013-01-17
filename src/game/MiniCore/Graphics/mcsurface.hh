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

#include <MCGLEW>

#include "mctypes.hh"
#include "mcmacros.hh"
#include "mcbbox.hh"
#include "mcvector2d.hh"
#include "mcvector3d.hh"

#include <cmath>
#include <string>

using std::string;

class MCCamera;
class MCGLShaderProgram;
class MCGLTexCoord;
class MCGLVertex;

/*! MCSurface is a renderable object bound to an OpenGL texture handle.
 *  MCSurface can be rendered straightly as a standalone object.
 */
class MCSurface
{
public:

    //! Constructor.
    //! \param handle Handle of the corresponding OpenGL texture 1.
    //! \param handle Handle of the corresponding OpenGL texture 2 (multitexturing) or zero.
    //! \param width  Desired width of the surface when rendered 1:1.
    //! \param height Desired height of the surface when rendered 1:1.
    //! \param z0     Z-coordinate for vertex[0]. Enables tilted surfaces.
    //! \param z1     Z-coordinate for vertex[1]. Enables tilted surfaces.
    //! \param z2     Z-coordinate for vertex[2]. Enables tilted surfaces.
    //! \param z3     Z-coordinate for vertex[3]. Enables tilted surfaces.
    MCSurface(
        GLuint handle1, GLuint handle2, MCFloat width, MCFloat height,
        MCFloat z0 = 0, MCFloat z1 = 0, MCFloat z2 = 0, MCFloat z3 = 0);

    //! Constructor.
    //! \param handle Handle of the corresponding OpenGL texture 1.
    //! \param handle Handle of the corresponding OpenGL texture 2 (multitexturing) or zero.
    //! \param width  Desired width of the surface when rendered 1:1.
    //! \param height Desired height of the surface when rendered 1:1.
    //! \param texCoords Array including texture coordinates of the four vertices.
    MCSurface(GLuint handle1, GLuint handle2, MCFloat width, MCFloat height,
        const MCGLTexCoord texCoords[4]);

    //! Destructor.
    virtual ~MCSurface();

    //! Set the point the surface is centered about.
    //! \param center Vector that defines a custom rotation point other than
    //!        (width / 2, height / 2) which is the default.
    void setCenter(MCVector2dFR center);

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

    //! Set color.
    void setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a = 1.0);

    //! Set scaling factors.
    void setScale(MCFloat x, MCFloat y, MCFloat z);

    //! Set scaling by dimensions.
    void setScale(MCFloat w, MCFloat h);

    /*! Render by using the default size.
     * \param pos The position.
     * \param wr Half of the wanted width.
     * \param hr Half of the wanted height.
     */
    void render(MCCamera * pCamera, MCVector3dFR pos, MCFloat angle,
        bool autoBind = true);

    //! Render (fake) shadow
    void renderShadow(MCCamera * pCamera, MCVector2dFR pos, MCFloat angle,
        bool autoBind = true);

    //! Render the vertex buffer only. bind() must be called separately.
    void render();

    /*! Manually enable/disable OpenGL client states and texturing environment.
     *  This can be used to save some function calls when rendering the same
     *  surface multiple times.
     *  \see render() */
    void bind(bool enable) const;

    /*! Manually enable/disable OpenGL client states and texturing environment.
     *  This can be used to save some function calls when rendering the same
     *  surface multiple times.
     *  \see renderShadow()
     */
    void bindShadow(bool enable) const;

    //! Bind the current texture.
    void bindTexture(bool bindOnlyFirstTexture = false) const;

    //! Set the shader program to be used.
    void setShaderProgram(MCGLShaderProgram * program);

    //! Set the shader program to be used for 2d shadows.
    void setShadowShaderProgram(MCGLShaderProgram * program);

    //! Get the shader program to be used.
    MCGLShaderProgram * shaderProgram() const;

    //! Get the shader program to be used for 2d shadows.
    MCGLShaderProgram * shadowShaderProgram() const;

    //! Get OpenGL texture handle
    GLuint handle1() const;

    //! Get OpenGL texture handle
    GLuint handle2() const;

    //! Get width
    MCFloat width() const;

    //! Get height
    MCFloat height() const;

    //! Get center
    MCVector2d<MCFloat> center() const;

private:

    DISABLE_COPY(MCSurface);
    DISABLE_ASSI(MCSurface);

    void init(GLuint handle1, GLuint handle2, MCFloat width, MCFloat height);

    void initVBOs(
        const MCGLVertex   * vertices,
        const MCGLVertex   * normals,
        const MCGLTexCoord * texCoords,
        const GLfloat      * colors);

    void doRender(bool autoBind);

    void doRenderShadow(bool autoBind);

    GLuint m_handle1;
    GLuint m_handle2;
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
    GLuint m_vbo;
    GLuint m_vba;
    GLfloat m_r, m_g, m_b, m_a;
    MCFloat m_sx, m_sy, m_sz;
    MCGLShaderProgram * m_program;
    MCGLShaderProgram * m_shadowProgram;
};

#endif // MCSURFACE_HH
