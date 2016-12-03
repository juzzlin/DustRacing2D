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
#include <memory>

class MCGLMaterial
{
public:

    static const unsigned int MAX_TEXTURES = 3;

    MCGLMaterial();

    //! Set texture handle or zero.
    void setTexture(GLuint handle, unsigned int index);

    //! \return given texture handle.
    GLuint texture(unsigned int index) const;

    //! Set coefficient for specular lighting.
    void setSpecularCoeff(GLfloat coeff);

    //! \return coefficient for specular lighting.
    GLfloat specularCoeff() const;

private:

    GLuint m_textures[MAX_TEXTURES];

    GLfloat m_specularCoeff;
};

typedef std::shared_ptr<MCGLMaterial> MCGLMaterialPtr;

#endif // MCGLMATERIAL_HH
