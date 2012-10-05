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

#ifndef MCGLRECTPARTICLE_HH
#define MCGLRECTPARTICLE_HH

#include "mcparticle.hh"

#include <GL/gl.h>

class MCCamera;
class MCGLShaderProgram;

/*! \class MCGLRectParticle
 *  \brief A particle that renders as a simple OpenGL rectangle. Final class.
 */
class MCGLRectParticle : public MCParticle
{
public:

    //! Constructor.
    MCGLRectParticle(const std::string & typeID);

    //! Destructor.
    virtual ~MCGLRectParticle();

    //! Set color
    void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0);

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

    enum VBOType
    {
        VBOVertex,
        VBONormal,
        VBOColor,
        VBOTypes
    };

    //! Set current alpha, doesn't affect the initial alpha
    //! set by setColor().
    void setAlpha(MCFloat a);

    DISABLE_COPY(MCGLRectParticle);
    DISABLE_ASSI(MCGLRectParticle);
    GLfloat m_r, m_g, m_b, m_a;
    GLuint m_vbos[VBOTypes];
    int m_frameCount;
    MCGLShaderProgram * m_program;
};

#endif // MCGLRECTPARTICLE_HH
