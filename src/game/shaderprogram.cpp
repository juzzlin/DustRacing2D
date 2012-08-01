// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "shaderprogram.hpp"

#include <cassert>

static const int POSITION = 1;
static const int ANGLE    = 5;
static const int SCALE    = 6;

ShaderProgram::ShaderProgram(const QGLContext * context)
: m_program(context)
, m_fragmentShader(QGLShader::Fragment, context)
, m_vertexShader(QGLShader::Vertex, context)
{
}

void ShaderProgram::bind()
{
    m_program.bind();
}

void ShaderProgram::release()
{
    m_program.release();
}

void ShaderProgram::link()
{
    m_program.link();
    assert(m_program.isLinked());
}

bool ShaderProgram::isLinked()
{
    return m_program.isLinked();
}

bool ShaderProgram::addVertexShader(const std::string & fileName)
{
    m_vertexShader.compileSourceFile(fileName.c_str());
    m_program.bindAttributeLocation("position", POSITION);
    m_program.bindAttributeLocation("angle",    ANGLE);
    m_program.bindAttributeLocation("scale",    SCALE);
    return m_program.addShader(&m_vertexShader);
}

bool ShaderProgram::addFragmentShader(const std::string & fileName)
{
    m_fragmentShader.compileSourceFile(fileName.c_str());
    return m_program.addShader(&m_fragmentShader);
}

void ShaderProgram::rotate(GLfloat angle)
{
    m_program.setAttributeValue(ANGLE, angle);
}

void ShaderProgram::translate(const MCVector3dF & p)
{
    m_program.setAttributeValue(POSITION, p.i(), p.j(), p.k(), 0);
}

void ShaderProgram::setColor(GLfloat, GLfloat, GLfloat, GLfloat)
{
}

void ShaderProgram::setScale(GLfloat x, GLfloat y, GLfloat z)
{
    m_program.setAttributeValue(SCALE, x, y, z, 1);
}

ShaderProgram::~ShaderProgram()
{
}

