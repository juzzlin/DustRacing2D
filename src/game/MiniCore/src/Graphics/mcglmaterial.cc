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

#include "mcglmaterial.hh"
#include <cassert>

MCGLMaterial::MCGLMaterial()
  : m_textures({})
  , m_specularCoeff(1.0)
  , m_diffuseCoeff(1.0)
  , m_useAlphaBlend(false)
  , m_src(GL_SRC_ALPHA)
  , m_dst(GL_ONE_MINUS_SRC_ALPHA)
{
}

void MCGLMaterial::setTexture(GLuint handle, size_t index)
{
    assert(index < MAX_TEXTURES);
    m_textures[index] = handle;
}

GLuint MCGLMaterial::texture(size_t index) const
{
    assert(index < MAX_TEXTURES);
    return m_textures[index];
}

void MCGLMaterial::setSpecularCoeff(GLfloat coeff)
{
    m_specularCoeff = coeff;
}

GLfloat MCGLMaterial::specularCoeff() const
{
    return m_specularCoeff;
}

void MCGLMaterial::setDiffuseCoeff(GLfloat coeff)
{
    m_diffuseCoeff = coeff;
}

GLfloat MCGLMaterial::diffuseCoeff() const
{
    return m_diffuseCoeff;
}

void MCGLMaterial::setAlphaBlend(bool useAlphaBlend, GLenum src, GLenum dst)
{
    m_useAlphaBlend = useAlphaBlend;
    m_src = src;
    m_dst = dst;
}

void MCGLMaterial::doAlphaBlend()
{
    if (m_useAlphaBlend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(m_src, m_dst);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}
