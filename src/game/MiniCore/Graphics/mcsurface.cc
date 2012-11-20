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

#include "mcsurface.hh"
#include "mccamera.hh"
#include "mcbbox.hh"
#include "mcglshaderprogram.hh"
#include "mcglvertex.hh"
#include "mcgltexcoord.hh"
#include "mctrigonom.hh"

#include <cassert>

static const int gNumVertices           = 6;
static const int gNumVertexComponents   = 3;
static const int gNumColorComponents    = 4;
static const int gNumTexCoordComponents = 2;

MCSurface::MCSurface(
    GLuint handle1, GLuint handle2, MCFloat width, MCFloat height,
    MCFloat z0, MCFloat z1, MCFloat z2, MCFloat z3)
{
    init(handle1, handle2, width, height);

    // Init vertice data for two triangles.
    const MCGLVertex vertices[gNumVertices] =
    {
        {-(GLfloat)m_w2, -(GLfloat)m_h2, z0},
        {-(GLfloat)m_w2,  (GLfloat)m_h2, z1},
        { (GLfloat)m_w2,  (GLfloat)m_h2, z2},
        {-(GLfloat)m_w2, -(GLfloat)m_h2, z0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, z2},
        { (GLfloat)m_w2, -(GLfloat)m_h2, z3}
    };

    const MCGLVertex normals[gNumVertices] =
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    };

    const MCGLTexCoord texCoords[gNumVertices] =
    {
        {0, 0},
        {0, 1},
        {1, 1},
        {0, 0},
        {1, 1},
        {1, 0}
    };

    const GLfloat colors[gNumVertices * gNumColorComponents] =
    {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
    };

    initVBOs(vertices, normals, texCoords, colors);
}

MCSurface::MCSurface(
    GLuint handle1, GLuint handle2, MCFloat width, MCFloat height, const MCGLTexCoord texCoords[4])
{
    init(handle1, handle2, width, height);

    // Init vertice data for two triangles.
    const MCGLVertex vertices[gNumVertices] =
    {
        {-(GLfloat)m_w2, -(GLfloat)m_h2, 0},
        {-(GLfloat)m_w2,  (GLfloat)m_h2, 0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, 0},
        {-(GLfloat)m_w2, -(GLfloat)m_h2, 0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, 0},
        { (GLfloat)m_w2, -(GLfloat)m_h2, 0}
    };

    const MCGLVertex normals[gNumVertices] =
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    };

    const GLfloat colors[gNumVertices * gNumColorComponents] =
    {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
    };

    initVBOs(vertices, normals, texCoords, colors);
}

void MCSurface::init(GLuint handle1, GLuint handle2, MCFloat width, MCFloat height)
{
    m_handle1        = handle1;
    m_handle2        = handle2;
    m_w              = width;
    m_w2             = width / 2;
    m_h              = height;
    m_h2             = height / 2;
    m_center         = MCVector2dF(m_w2, m_h2);
    m_centerSet      = false;
    m_useAlphaTest   = false;
    m_alphaFunc      = GL_ALWAYS;
    m_alphaThreshold = 0.0;
    m_useAlphaBlend  = false;
    m_src            = GL_SRC_ALPHA;
    m_dst            = GL_ONE_MINUS_SRC_ALPHA;
    m_program        = nullptr;
    m_shadowProgram  = nullptr;
}

void MCSurface::initVBOs(
    const MCGLVertex   * vertices,
    const MCGLVertex   * normals,
    const MCGLTexCoord * texCoords,
    const GLfloat      * colors)
{
    glGenBuffers(VBOTypes, m_vbos);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOVertex]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(MCGLVertex) * gNumVertices, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBONormal]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(MCGLVertex) * gNumVertices, normals, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOTexture]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(MCGLTexCoord) * gNumVertices, texCoords, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * gNumVertices * gNumColorComponents,
        colors, GL_DYNAMIC_DRAW);
}

MCSurface::~MCSurface()
{
    glDeleteBuffers(VBOTypes, m_vbos);
}

void MCSurface::setCenter(MCVector2dFR center)
{
    m_centerSet = true;
    m_center    = center;
}

void MCSurface::setAlphaBlend(
    bool useAlphaBlend, GLenum src, GLenum dst)
{
    m_useAlphaBlend  = useAlphaBlend;
    m_src            = src;
    m_dst            = dst;
}

void MCSurface::doAlphaBlend() const
{
    if (m_useAlphaBlend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(m_src, m_dst);
    }
}

void MCSurface::setTexCoords(const MCGLTexCoord texCoords[4])
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOTexture]);

    // This should make things a bit faster especially on NVIDIA.
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(MCGLTexCoord) * gNumVertices, nullptr, GL_DYNAMIC_DRAW);

    MCGLTexCoord * pTexData = (MCGLTexCoord *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (pTexData)
    {
        pTexData[0] = texCoords[0];
        pTexData[1] = texCoords[1];
        pTexData[2] = texCoords[2];
        pTexData[3] = texCoords[0];
        pTexData[4] = texCoords[2];
        pTexData[5] = texCoords[3];

        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
}

void MCSurface::setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);

    // This should make things a bit faster especially on NVIDIA.
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * gNumVertices * gNumColorComponents, nullptr, GL_DYNAMIC_DRAW);

    GLfloat * pColorData = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (pColorData)
    {
        for (int i = 0; i < gNumVertices; i++)
        {
            const int offset = (i << 2);
            pColorData[offset + 0] = r;
            pColorData[offset + 1] = g;
            pColorData[offset + 2] = b;
            pColorData[offset + 3] = a;
        }

        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
}

MCBBox<MCFloat> MCSurface::rotatedBBox(MCVector2dFR pos, MCFloat angle)
{
    using std::abs;

    const MCFloat cos = MCTrigonom::cos(angle);
    const MCFloat sin = MCTrigonom::sin(angle);
    const MCFloat  w1 = (abs(cos * m_w2) + abs(sin * m_h2)) / 2;
    const MCFloat  h1 = (abs(sin * m_w2) + abs(cos * m_h2)) / 2;

    return MCBBox<MCFloat>(pos.i() - w1, pos.j() - h1, pos.i() + w1, pos.j() + h1);
}

MCBBox<MCFloat> MCSurface::rotatedScaledBBox(
    MCVector2dFR pos, MCFloat angle, MCFloat w2, MCFloat h2)
{
    using std::abs;

    const MCFloat cos = MCTrigonom::cos(angle);
    const MCFloat sin = MCTrigonom::sin(angle);
    const MCFloat  w1 = (abs(cos * w2) + abs(sin * h2)) / 2;
    const MCFloat  h1 = (abs(sin * w2) + abs(cos * h2)) / 2;

    return MCBBox<MCFloat>(pos.i() - w1, pos.j() - h1, pos.i() + w1, pos.j() + h1);
}

void MCSurface::renderVBOs(bool autoClientState)
{
    if (autoClientState)
    {
        enableClientState(true);
        bindTexture();
    }

    glDrawArrays(GL_TRIANGLES, 0, gNumVertices);

    if (autoClientState)
    {
        enableClientState(false);
    }
}

void MCSurface::renderShadowVBOs(bool autoClientState)
{
    if (autoClientState)
    {
        enableShadowClientState(true);
        bindTexture(true);
    }

    glDrawArrays(GL_TRIANGLES, 0, gNumVertices);

    if (autoClientState)
    {
        enableShadowClientState(false);
    }
}

void MCSurface::bindTexture(bool bindOnlyFirstTexture) const
{
    assert(m_program);

    if (m_handle2 && !bindOnlyFirstTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_handle1);
        m_program->bindTextureUnit0(0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_handle2);
        m_program->bindTextureUnit1(1);
        glActiveTexture(GL_TEXTURE0);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, m_handle1);
    }
}

void MCSurface::setShaderProgram(MCGLShaderProgram * program)
{
    m_program = program;
}

void MCSurface::setShadowShaderProgram(MCGLShaderProgram * program)
{
    m_shadowProgram = program;
}

MCGLShaderProgram * MCSurface::shaderProgram() const
{
    return m_program;
}

MCGLShaderProgram * MCSurface::shadowShaderProgram() const
{
    return m_shadowProgram;
}

void MCSurface::render(MCCamera * pCamera, MCVector3dFR pos, MCFloat angle,
    bool autoClientState)
{
    if (m_program)
    {
        MCFloat x = pos.i();
        MCFloat y = pos.j();
        MCFloat z = pos.k();

        if (pCamera)
        {
            pCamera->mapToCamera(x, y);
        }

        doAlphaBlend();

        m_program->bind();
        m_program->setScale(1.0, 1.0, 1.0);

        if (m_centerSet)
        {
            m_program->translate(MCVector3dF(x + m_w2 - m_center.i(), y + m_h2 - m_center.j(), z));
        }
        else
        {
            m_program->translate(MCVector3dF(x, y, z));
        }

        m_program->rotate(angle);

        renderVBOs(autoClientState);

        m_program->release();

        if (m_useAlphaBlend)
        {
            glDisable(GL_BLEND);
        }
    }
}

void MCSurface::renderScaled(
    MCCamera * pCamera, MCVector3dFR pos, MCFloat wr, MCFloat hr, MCFloat angle,
    bool autoClientState)
{
    if (m_program)
    {
        MCFloat x = pos.i();
        MCFloat y = pos.j();
        MCFloat z = pos.k();

        if (pCamera)
        {
            pCamera->mapToCamera(x, y);
        }

        doAlphaBlend();

        m_program->bind();
        m_program->setScale(wr / m_w2, hr / m_h2, 1.0);

        if (m_centerSet)
        {
            m_program->translate(
                MCVector3dF(x + m_w2 - m_center.i(), y + m_h2 - m_center.j(), z));
        }
        else
        {
            m_program->translate(MCVector3dF(x, y, z));
        }

        m_program->rotate(angle);

        renderVBOs(autoClientState);

        m_program->release();

        if (m_useAlphaBlend)
        {
            glDisable(GL_BLEND);
        }
    }
}

void MCSurface::renderShadow(MCCamera * pCamera, MCVector2dFR pos, MCFloat angle,
    bool autoClientState)
{
    if (m_shadowProgram)
    {
        MCFloat x = pos.i();
        MCFloat y = pos.j();
        MCFloat z = 0;

        if (pCamera)
        {
            pCamera->mapToCamera(x, y);
        }

        m_shadowProgram->bind();
        m_shadowProgram->setScale(1.0, 1.0, 1.0);

        if (m_centerSet)
        {
            m_shadowProgram->translate(
                MCVector3dF(x + m_w2 - m_center.i(), y + m_h2 - m_center.j(), z));
        }
        else
        {
            m_shadowProgram->translate(MCVector3dF(x, y, z));
        }

        m_shadowProgram->rotate(angle);

        renderShadowVBOs(autoClientState);

        m_shadowProgram->release();
    }
}

void MCSurface::renderShadowScaled(
    MCCamera * pCamera, MCVector2dFR pos, MCFloat wr, MCFloat hr, MCFloat angle,
    bool autoClientState)
{
    if (m_shadowProgram)
    {
        MCFloat x = pos.i();
        MCFloat y = pos.j();
        MCFloat z = 0;

        if (pCamera)
        {
            pCamera->mapToCamera(x, y);
        }

        m_shadowProgram->bind();
        m_shadowProgram->setScale(wr / m_w2, hr / m_h2, 1.0);

        if (m_centerSet)
        {
            m_shadowProgram->translate(
                MCVector3dF(x + m_w2 - m_center.i(), y + m_h2 - m_center.j(), z));
        }
        else
        {
            m_shadowProgram->translate(MCVector3dF(x, y, z));
        }

        m_shadowProgram->rotate(angle);

        renderShadowVBOs(autoClientState);

        m_shadowProgram->release();
    }
}

void MCSurface::enableClientState(bool enable) const
{
    if (enable)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOVertex]);
        glVertexPointer(gNumVertexComponents, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBONormal]);
        glNormalPointer(GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOTexture]);
        glTexCoordPointer(gNumTexCoordComponents, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);
        glColorPointer(gNumColorComponents, GL_FLOAT, 0, 0);

        bindTexture();
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_program->bindTextureUnit0(0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_program->bindTextureUnit1(0);

        glActiveTexture(GL_TEXTURE0);
    }
}

void MCSurface::enableShadowClientState(bool enable) const
{
    if (enable)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOVertex]);
        glVertexPointer(gNumVertexComponents, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOTexture]);
        glTexCoordPointer(gNumTexCoordComponents, GL_FLOAT, 0, 0);

        bindTexture(true);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

GLuint MCSurface::handle1() const
{
    return m_handle1;
}

GLuint MCSurface::handle2() const
{
    return m_handle2;
}

MCFloat MCSurface::width() const
{
    return m_w;
}

MCFloat MCSurface::height() const
{
    return m_h;
}

MCVector2dF MCSurface::center() const
{
    return m_center;
}
