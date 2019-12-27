// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcglobjectbase.hh"

#include "mccamera.hh"
#include "mcglscene.hh"
#include "mclogger.hh"

#include <cassert>
#include <exception>

GLuint MCGLObjectBase::m_boundVbo = 0;

MCGLObjectBase::MCGLObjectBase(std::string handle)
  : m_handle(handle)
  , m_program(MCGLScene::instance().defaultShaderProgram())
  , m_shadowProgram(MCGLScene::instance().defaultShadowShaderProgram())
{
#ifdef __MC_QOPENGLFUNCTIONS__
    initializeOpenGLFunctions();
#endif
}

void MCGLObjectBase::setShaderProgram(MCGLShaderProgramPtr program)
{
    m_program = program;
}

void MCGLObjectBase::setShadowShaderProgram(MCGLShaderProgramPtr program)
{
    m_shadowProgram = program;
}

MCGLShaderProgramPtr MCGLObjectBase::shaderProgram() const
{
    return m_program;
}

MCGLShaderProgramPtr MCGLObjectBase::shadowShaderProgram() const
{
    return m_shadowProgram;
}

// Notice that on desktop OpenGL 3.0+ VAOs are mandatory.
// Without VAOs this code will run only on GLES.
void MCGLObjectBase::bindVAO()
{
#ifdef __MC_QOPENGLFUNCTIONS__
    if (m_hasVao)
    {
        m_vao.bind();
    }
    else
    {
        setAttributePointers();
    }
#else
    glBindVertexArray(m_vao);
#endif
}

void MCGLObjectBase::releaseVAO()
{
#ifdef __MC_QOPENGLFUNCTIONS__
    if (m_hasVao)
    {
        m_vao.release();
    }
#else
    glBindVertexArray(0);
#endif
}

bool MCGLObjectBase::createVAO()
{
#ifdef __MC_QOPENGLFUNCTIONS__
    m_hasVao = m_vao.create();
    return m_hasVao;
#else
    if (m_vao == 0)
    {
        glGenVertexArrays(1, &m_vao);
    }
    return m_vao;
#endif
}

void MCGLObjectBase::bindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    MCGLObjectBase::m_boundVbo = m_vbo;
}

void MCGLObjectBase::releaseVBO()
{
    MCGLObjectBase::m_boundVbo = 0;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MCGLObjectBase::createVBO()
{
    if (m_vbo == 0)
    {
        glGenBuffers(1, &m_vbo);
    }
}

void MCGLObjectBase::render()
{
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(m_vertices.size()));
}

void MCGLObjectBase::render(MCCamera * camera, MCVector3dFR pos, float angle)
{
    float x = pos.i();
    float y = pos.j();
    float z = pos.k();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    bind();

    shaderProgram()->setScale(m_scale.i(), m_scale.j(), m_scale.k());
    shaderProgram()->setColor(color());
    shaderProgram()->setTransform(angle, MCVector3dF(x, y, z));

    render();

    release();
}

void MCGLObjectBase::renderShadow(MCCamera * camera, MCVector3dFR pos, float angle)
{
    float x = pos.i();
    float y = pos.j();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    bindShadow();

    shadowShaderProgram()->setScale(m_scale.i(), m_scale.j(), m_scale.k());
    shadowShaderProgram()->setTransform(angle, MCVector3dF(x, y, pos.k()));

    render();

    releaseShadow();
}

void MCGLObjectBase::bind()
{
    if (shaderProgram())
    {
        shaderProgram()->bind();
        shaderProgram()->bindMaterial(m_material);

        bindVBO();
        bindVAO();
    }
    else
    {
        // Save the user from debugging as to why nothing is being drawn.
        throw std::runtime_error("Trying to bind MCGLObject but shader program for it not set!");
    }
}

void MCGLObjectBase::bindShadow()
{
    if (shadowShaderProgram())
    {
        shadowShaderProgram()->bind();
        shadowShaderProgram()->bindMaterial(m_material);

        bindVBO();
        bindVAO();
    }
    else
    {
        // Save the user from debugging as to why nothing is being drawn.
        throw std::runtime_error("Trying to render shadow for surface, but shader program for it not set!");
    }
}

void MCGLObjectBase::release()
{
    releaseVAO();
}

void MCGLObjectBase::releaseShadow()
{
    releaseVAO();
}

void MCGLObjectBase::setMaterial(MCGLMaterialPtr material)
{
    m_material = material;
}

MCGLMaterialPtr MCGLObjectBase::material() const
{
    return m_material;
}

void MCGLObjectBase::initBufferData(size_t totalDataSize, GLuint drawType)
{
    m_totalDataSize = totalDataSize;

    createVAO();
    createVBO();

    bindVAO();
    bindVBO();

    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(m_totalDataSize), nullptr, drawType);

    m_bufferDataOffset = 0;
}

void MCGLObjectBase::addVertex(const MCGLVertex & vertex)
{
    m_vertices.push_back(vertex);
}

void MCGLObjectBase::setVertices(const VertexVector & vertices)
{
    m_vertices = vertices;
}

const MCGLVertex & MCGLObjectBase::vertex(size_t index) const
{
    return m_vertices.at(index);
}

const GLfloat * MCGLObjectBase::verticesAsGlArray() const
{
    return reinterpret_cast<const GLfloat *>(&m_vertices[0]);
}

size_t MCGLObjectBase::vertexCount() const
{
    return m_vertices.size();
}

void MCGLObjectBase::addNormal(const MCGLVertex & normal)
{
    m_normals.push_back(normal);
}

void MCGLObjectBase::setNormals(const VertexVector & normals)
{
    m_normals = normals;
}

const MCGLVertex & MCGLObjectBase::normal(size_t index) const
{
    return m_normals.at(index);
}

const GLfloat * MCGLObjectBase::normalsAsGlArray() const
{
    return reinterpret_cast<const GLfloat *>(&m_normals[0]);
}

void MCGLObjectBase::setColor(const MCGLColor & color)
{
    m_color = color;
}

MCGLColor MCGLObjectBase::color() const
{
    return m_color;
}

void MCGLObjectBase::addColor(const MCGLColor & color)
{
    m_colors.push_back(color);
}

void MCGLObjectBase::setColors(const ColorVector & colors)
{
    m_colors = colors;
}

const MCGLColor & MCGLObjectBase::color(size_t index) const
{
    return m_colors.at(index);
}

const GLfloat * MCGLObjectBase::colorsAsGlArray() const
{
    return reinterpret_cast<const GLfloat *>(&m_colors[0]);
}

void MCGLObjectBase::addTexCoord(const MCGLTexCoord & texCoord)
{
    m_texCoords.push_back(texCoord);
}

void MCGLObjectBase::setTexCoords(const TexCoordVector & texCoords)
{
    m_texCoords = texCoords;
}

const MCGLTexCoord & MCGLObjectBase::texCoord(size_t index) const
{
    return m_texCoords.at(index);
}

const GLfloat * MCGLObjectBase::texCoordsAsGlArray() const
{
    return reinterpret_cast<const GLfloat *>(&m_texCoords[0]);
}

void MCGLObjectBase::initUpdateBufferData()
{
    bindVAO();
    bindVBO();

    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(m_totalDataSize), nullptr, GL_DYNAMIC_DRAW);

    m_bufferDataOffset = 0;
}

void MCGLObjectBase::addBufferSubData(
  MCGLShaderProgram::VertexAttribLocations dataType, size_t dataSize, const GLfloat * data)
{
    addBufferSubData(dataType, dataSize, dataSize, data);
}

void MCGLObjectBase::addBufferSubData(
  MCGLShaderProgram::VertexAttribLocations dataType, size_t dataSize, size_t offsetJump, const GLfloat * data)
{
    assert(dataSize <= offsetJump);

    glBufferSubData(GL_ARRAY_BUFFER, static_cast<int>(m_bufferDataOffset), static_cast<int>(dataSize), data);

    m_bufferDataOffset += offsetJump;

    switch (dataType)
    {
    case MCGLShaderProgram::VAL_Vertex:
        m_vertexDataSize = dataSize;
        break;
    case MCGLShaderProgram::VAL_Normal:
        m_normalDataSize = dataSize;
        break;
    case MCGLShaderProgram::VAL_TexCoords:
        m_texCoordDataSize = dataSize;
        break;
    case MCGLShaderProgram::VAL_Color:
        m_colorDataSize = dataSize;
        break;
    }
}

void MCGLObjectBase::enableAttributePointers()
{
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Normal);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_TexCoords);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Color);
}

void MCGLObjectBase::setAttributePointers()
{
    enableAttributePointers();

    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glVertexAttribPointer(MCGLShaderProgram::VAL_Normal, 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid *>(m_vertexDataSize));

    glVertexAttribPointer(MCGLShaderProgram::VAL_TexCoords, 2, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid *>(m_vertexDataSize + m_normalDataSize));

    glVertexAttribPointer(MCGLShaderProgram::VAL_Color, 4, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid *>(m_vertexDataSize + m_normalDataSize + m_texCoordDataSize));
}

void MCGLObjectBase::disableAttributePointers()
{
    glDisableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
    glDisableVertexAttribArray(MCGLShaderProgram::VAL_Normal);
    glDisableVertexAttribArray(MCGLShaderProgram::VAL_TexCoords);
    glDisableVertexAttribArray(MCGLShaderProgram::VAL_Color);
}

MCVector3dF MCGLObjectBase::scale() const
{
    return m_scale;
}

std::string MCGLObjectBase::handle() const
{
    return m_handle;
}

void MCGLObjectBase::setHandle(const std::string & handle)
{
    m_handle = handle;
}

float MCGLObjectBase::width() const
{
    return m_width * m_scale.i();
}

void MCGLObjectBase::setWidth(float width)
{
    m_width = width;
}

float MCGLObjectBase::height() const
{
    return m_height * m_scale.j();
}

void MCGLObjectBase::setHeight(float height)
{
    m_height = height;
}

void MCGLObjectBase::setMaxZ(float maxZ)
{
    m_maxZ = maxZ;
}

void MCGLObjectBase::setMinZ(float minZ)
{
    m_minZ = minZ;
}

void MCGLObjectBase::setScale(const MCVector3dF & scale)
{
    m_scale = scale;
}

void MCGLObjectBase::setSize(float width, float height)
{
    m_scale.setI(width / m_width);
    m_scale.setJ(height / m_height);
}

float MCGLObjectBase::minZ() const
{
    return m_minZ * m_scale.k();
}

float MCGLObjectBase::maxZ() const
{
    return m_maxZ * m_scale.k();
}

void MCGLObjectBase::finishBufferData()
{
    setAttributePointers();

    releaseVBO();
    releaseVAO();
}

size_t MCGLObjectBase::totalDataSize() const
{
    return m_totalDataSize;
}

MCGLObjectBase::~MCGLObjectBase()
{
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
#ifndef __MC_QOPENGLFUNCTIONS__
    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
#endif
}
