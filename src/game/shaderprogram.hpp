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

#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <MCGLShaderProgram>
#include <QGLShaderProgram>
#include <QGLShader>
#include <QGLContext>
#include <QObject>

class ShaderProgram : public MCGLShaderProgram
{
public:
    ShaderProgram(const QGLContext * context);

    virtual ~ShaderProgram();

    virtual void bind();

    virtual void release();

    virtual void link();

    virtual bool isLinked() const;

    virtual bool addVertexShader(const std::string & path);

    virtual bool addFragmentShader(const std::string & path);

    virtual void rotate(GLfloat angle);

    virtual void translate(const MCVector3dF & pos);

    virtual void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

    virtual void setScale(GLfloat x, GLfloat y, GLfloat z);

    virtual void setFadeValue(GLfloat f);

private:

    QGLShaderProgram m_program;
    QGLShader        m_fragmentShader;
    QGLShader        m_vertexShader;
};

#endif // SHADERPROGRAM_HPP
