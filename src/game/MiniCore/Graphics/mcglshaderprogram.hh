// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCGLSHADERPROGRAM_HH
#define MCGLSHADERPROGRAM_HH

#include "mctypes.hh"
#include "mcvector3d.hh"
#include <GL/gl.h>
#include <string>

class MCGLShaderProgram
{
public:

    //! Constructor.
    MCGLShaderProgram();

    //! Destructor.
    virtual ~MCGLShaderProgram();

    virtual void bind();

    virtual void release();

    virtual void link();

    virtual bool isLinked();

    virtual bool addVertexShader(const std::string & path);

    virtual bool addFragmentShader(const std::string & path);

    virtual void rotate(GLfloat angle);

    virtual void translate(const MCVector3dF & pos);

    virtual void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

    virtual void setScale(GLfloat x, GLfloat y, GLfloat z);

private:
};

#endif // MCGLSHADERPROGRAM_HH
