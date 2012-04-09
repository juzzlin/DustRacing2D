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

#include "mcglrectparticle.hh"
#include "mcglrectparticleimpl.hh"
#include "mccamera.hh"

#include <GL/gl.h>

MCRecycler<MCGLRectParticle> MCGLRectParticle::m_recycler;

MCGLRectParticleImpl::MCGLRectParticleImpl()
: m_r(1.0f)
, m_g(1.0f)
, m_b(1.0f)
, m_a(1.0f)
{}

MCGLRectParticleImpl::~MCGLRectParticleImpl()
{}

MCGLRectParticle::MCGLRectParticle() :
    m_pImpl(new MCGLRectParticleImpl)
{
    // Disable shadow by default
    setHasShadow(false);
}

void MCGLRectParticle::setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a)
{
    m_pImpl->m_r = r;
    m_pImpl->m_g = g;
    m_pImpl->m_b = b;
    m_pImpl->m_a = a;
}

void MCGLRectParticle::render(MCCamera * pCamera)
{
    MCFloat x = location().i();
    MCFloat y = location().j();

    if (pCamera) {
        pCamera->mapToCamera(x, y);
    }

    // Disable texturing
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslated(x, y, location().k());

    // Rotate
    if (angle()) {
        glRotated(angle(), 0, 0, 1);
    }

    // Enable blending
    MCFloat alpha = m_pImpl->m_a;
    if (alpha < 1.0f || animationStyle() == FadeOut) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // Scale alpha if fading out
    if (animationStyle() == FadeOut) {
        alpha *= scale();
    }

    // Scale radius if fading out
    MCFloat r = radius();
    if (animationStyle() == Shrink) {
        r *= scale();
    }

    if (r > 0)
    {
        glColor4f(m_pImpl->m_r, m_pImpl->m_g, m_pImpl->m_b, alpha);
        glNormal3f(0, 0, 1.0f);
        glPointSize(r + r);
        glBegin(GL_POINTS);
        glVertex2f(0, 0);
        glEnd();
    }

    glPopMatrix();
    glPopAttrib();
}

void MCGLRectParticle::renderShadow(MCCamera *)
{
    return;
}

MCGLRectParticle & MCGLRectParticle::create()
{
    return *MCGLRectParticle::m_recycler.newObject();
}

void MCGLRectParticle::recycle()
{
    MCGLRectParticle::m_recycler.freeObject(this);
}

MCGLRectParticle::~MCGLRectParticle()
{
    delete m_pImpl;
}
