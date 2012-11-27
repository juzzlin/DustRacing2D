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

#ifndef MCGLPOINTPARTICLE_HH
#define MCGLPOINTPARTICLE_HH

#include <MCGLEW>
#include "mcparticle.hh"

class MCCamera;
class MCGLShaderProgram;

/*! \class MCGLRectParticle
 *  \brief A particle that renders as a simple OpenGL point. Final class.
 */
class MCGLPointParticle : public MCParticle
{
public:

    //! Constructor.
    MCGLPointParticle(const std::string & typeID);

    //! Destructor.
    virtual ~MCGLPointParticle();

    //! Set color
    void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0);

    //! Set the shader program to be used.
    void setShaderProgram(MCGLShaderProgram * program);

    //! \reimp
    void render(MCCamera * pCamera = nullptr);

    //! \reimp
    void renderShadow(MCCamera * pCamera = nullptr);

    //! \reimp
    void beginBatch();

    //! \reimp
    void endBatch();

private:

    //! Set current alpha, doesn't affect the initial alpha
    //! set by setColor().
    void setAlpha(MCFloat a);

    DISABLE_COPY(MCGLPointParticle);
    DISABLE_ASSI(MCGLPointParticle);
    GLfloat m_r, m_g, m_b, m_a;
    GLuint m_vbo;
    GLuint m_vba;
    int m_frameCount;
    MCGLShaderProgram * m_program;
};

#endif // MCGLPOINTPARTICLE_HH
