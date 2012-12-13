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

static const int NUM_VERTICES             = 6;
static const int NUM_VERTEX_COMPONENTS    = 3;
static const int NUM_COLOR_COMPONENTS     = 4;
static const int NUM_TEX_COORD_COMPONENTS = 2;

static const int VERTEX_DATA_SIZE         = sizeof(MCGLVertex)   * NUM_VERTICES;
static const int NORMAL_DATA_SIZE         = sizeof(MCGLVertex)   * NUM_VERTICES;
static const int TEXCOORD_DATA_SIZE       = sizeof(MCGLTexCoord) * NUM_VERTICES;
static const int COLOR_DATA_SIZE          = sizeof(GLfloat)      * NUM_VERTICES * NUM_COLOR_COMPONENTS;
static const int TOTAL_DATA_SIZE          =
    VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE + COLOR_DATA_SIZE;

MCSurface::MCSurface(
    GLuint handle1, GLuint handle2, MCFloat width, MCFloat height,
    MCFloat z0, MCFloat z1, MCFloat z2, MCFloat z3)
{
    init(handle1, handle2, width, height);

    // Init vertice data for two triangles.
    const MCGLVertex vertices[NUM_VERTICES] =
    {
        {-(GLfloat)m_w2, -(GLfloat)m_h2, z0},
        {-(GLfloat)m_w2,  (GLfloat)m_h2, z1},
        { (GLfloat)m_w2,  (GLfloat)m_h2, z2},
        {-(GLfloat)m_w2, -(GLfloat)m_h2, z0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, z2},
        { (GLfloat)m_w2, -(GLfloat)m_h2, z3}
    };

    const MCGLVertex normals[NUM_VERTICES] =
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    };

    const MCGLTexCoord texCoords[NUM_VERTICES] =
    {
        {0, 0},
        {0, 1},
        {1, 1},
        {0, 0},
        {1, 1},
        {1, 0}
    };

    const GLfloat colors[COLOR_DATA_SIZE] =
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
    const MCGLVertex vertices[NUM_VERTICES] =
    {
        {-(GLfloat)m_w2, -(GLfloat)m_h2, 0},
        {-(GLfloat)m_w2,  (GLfloat)m_h2, 0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, 0},
        {-(GLfloat)m_w2, -(GLfloat)m_h2, 0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, 0},
        { (GLfloat)m_w2, -(GLfloat)m_h2, 0}
    };

    const MCGLVertex normals[NUM_VERTICES] =
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    };

    const GLfloat colors[NUM_VERTICES * NUM_COLOR_COMPONENTS] =
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
    m_r              = 1.0;
    m_g              = 1.0;
    m_b              = 1.0;
    m_a              = 1.0;
}

void MCSurface::initVBOs(
    const MCGLVertex   * vertices,
    const MCGLVertex   * normals,
    const MCGLTexCoord * texCoords,
    const GLfloat      * colors)
{
    int offset = 0;

    glGenVertexArrays(1, &m_vba);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vba);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
        TOTAL_DATA_SIZE, nullptr, GL_STATIC_DRAW);

    // Vertex data
    glBufferSubData(GL_ARRAY_BUFFER, offset, VERTEX_DATA_SIZE, vertices);
    offset += VERTEX_DATA_SIZE;

    // Normal data
    glBufferSubData(GL_ARRAY_BUFFER, offset, NORMAL_DATA_SIZE, normals);
    offset += NORMAL_DATA_SIZE;

    // Texture coordinate data
    glBufferSubData(GL_ARRAY_BUFFER, offset, TEXCOORD_DATA_SIZE, texCoords);
    offset += TEXCOORD_DATA_SIZE;

    // Vertex color data
    glBufferSubData(GL_ARRAY_BUFFER, offset, COLOR_DATA_SIZE, colors);

    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex,    3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(MCGLShaderProgram::VAL_Normal,    3, GL_FLOAT, GL_FALSE, 0,
        (GLvoid *)VERTEX_DATA_SIZE);
    glVertexAttribPointer(MCGLShaderProgram::VAL_TexCoords, 2, GL_FLOAT, GL_FALSE, 0,
        (GLvoid *)(VERTEX_DATA_SIZE + NORMAL_DATA_SIZE));
    glVertexAttribPointer(MCGLShaderProgram::VAL_Color,     4, GL_FLOAT, GL_FALSE, 0,
        (GLvoid *)(VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE));

    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Normal);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_TexCoords);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Color);
}

MCSurface::~MCSurface()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vba);
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
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    const MCGLTexCoord texCoordsAll[NUM_VERTICES] =
    {
        texCoords[0],
        texCoords[1],
        texCoords[2],
        texCoords[0],
        texCoords[2],
        texCoords[3]
    };

    glBufferSubData(
        GL_ARRAY_BUFFER, VERTEX_DATA_SIZE + NORMAL_DATA_SIZE, TEXCOORD_DATA_SIZE, texCoordsAll);
}

void MCSurface::setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
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

void MCSurface::render()
{
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
}

void MCSurface::doRender(bool autoBind)
{
    if (autoBind)
    {
        bind(true);
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
        bind(false);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
    }
}

void MCSurface::doRenderShadow(bool autoBind)
{
    if (autoBind)
    {
        bindShadow(true);
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
        bindShadow(false);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
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
    bool autoBind)
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
        m_program->setColor(m_r, m_g, m_b, m_a);

        if (m_centerSet)
        {
            m_program->translate(MCVector3dF(x + m_w2 - m_center.i(), y + m_h2 - m_center.j(), z));
        }
        else
        {
            m_program->translate(MCVector3dF(x, y, z));
        }

        m_program->rotate(angle);

        doRender(autoBind);

        if (m_useAlphaBlend)
        {
            glDisable(GL_BLEND);
        }
    }
}

void MCSurface::renderScaled(
    MCCamera * pCamera, MCVector3dFR pos, MCFloat wr, MCFloat hr, MCFloat angle,
    bool autoBind)
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
        m_program->setColor(m_r, m_g, m_b, m_a);

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

        doRender(autoBind);

        if (m_useAlphaBlend)
        {
            glDisable(GL_BLEND);
        }
    }
}

void MCSurface::renderShadow(MCCamera * pCamera, MCVector2dFR pos, MCFloat angle,
    bool autoBind)
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

        doRenderShadow(autoBind);
    }
}

void MCSurface::renderShadowScaled(
    MCCamera * pCamera, MCVector2dFR pos, MCFloat wr, MCFloat hr, MCFloat angle,
    bool autoBind)
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

        doRenderShadow(autoBind);
    }
}

void MCSurface::bind(bool enable) const
{
    if (enable)
    {
        glBindVertexArray(m_vba);
        bindTexture();
    }
    else
    {
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_program->bindTextureUnit0(0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_program->bindTextureUnit1(0);

        glActiveTexture(GL_TEXTURE0);
    }
}

void MCSurface::bindShadow(bool enable) const
{
    if (enable)
    {
        glBindVertexArray(m_vba);
        bindTexture(true);
    }
    else
    {
        glBindVertexArray(0);
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
