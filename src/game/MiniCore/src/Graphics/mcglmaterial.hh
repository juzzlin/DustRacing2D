// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCGLMATERIAL_HH
#define MCGLMATERIAL_HH

#include <MCGLEW>

#include <array>
#include <memory>

class MCGLMaterial
{
public:
    static const size_t MAX_TEXTURES = 3;

    MCGLMaterial();

    //! Set texture handle or zero.
    void setTexture(GLuint handle, size_t index);

    //! \return given texture handle.
    GLuint texture(size_t index) const;

    //! Set coefficient for specular lighting.
    void setSpecularCoeff(GLfloat coeff);

    //! \return coefficient for specular lighting.
    GLfloat specularCoeff() const;

    //! Set coefficient for diffuse lighting.
    void setDiffuseCoeff(GLfloat coeff);

    //! \return coefficient for diffuse lighting.
    GLfloat diffuseCoeff() const;

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

private:
    std::array<GLuint, MAX_TEXTURES> m_textures;

    GLfloat m_specularCoeff;

    GLfloat m_diffuseCoeff;

    bool m_useAlphaBlend;

    GLenum m_src;

    GLenum m_dst;
};

typedef std::shared_ptr<MCGLMaterial> MCGLMaterialPtr;

#endif // MCGLMATERIAL_HH
