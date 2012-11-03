// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <MCGLShaderProgram>
#include <QGLShaderProgram>
#include <QGLShader>
#include <QGLContext>
#include <QObject>

/*! Implements the MCGLShaderProgram interface by wrapping
 *  a QGLShaderProgram. */
class ShaderProgram : public MCGLShaderProgram
{
public:

    //! Constructor.
    ShaderProgram(const QGLContext * context);

    //! Destructor.
    virtual ~ShaderProgram();

    //! \reimp
    virtual void bind();

    //! \reimp
    virtual void release();

    //! \reimp
    virtual void link();

    //! \reimp
    virtual bool isLinked() const;

    //! \reimp
    virtual bool addVertexShader(const std::string & path);

    //! \reimp
    virtual bool addFragmentShader(const std::string & path);

    //! \reimp
    virtual void rotate(GLfloat angle);

    //! \reimp
    virtual void translate(const MCVector3dF & pos);

    //! \reimp
    virtual void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

    //! \reimp
    virtual void setScale(GLfloat x, GLfloat y, GLfloat z);

    //! \reimp
    virtual void setFadeValue(GLfloat f);

private:

    QGLShaderProgram m_program;
    QGLShader        m_fragmentShader;
    QGLShader        m_vertexShader;
};

#endif // SHADERPROGRAM_HPP
