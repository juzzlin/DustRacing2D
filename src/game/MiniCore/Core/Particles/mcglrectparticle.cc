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
#include "../mccamera.hh"

#include <GL/gl.h>

MCRecycler<MCGLRectParticle> MCGLRectParticle::m_recycler;

MCGLRectParticleImpl::MCGLRectParticleImpl()
: r(1.0f)
, g(1.0f)
, b(1.0f)
, a(1.0f)
, group(nullptr)
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
    m_pImpl->r = r;
    m_pImpl->g = g;
    m_pImpl->b = b;
    m_pImpl->a = a;
}

void MCGLRectParticle::render(MCCamera * pCamera)
{
    // Disable texturing
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderInner(pCamera);

    glPopAttrib();
}

void MCGLRectParticle::renderInner(MCCamera * pCamera)
{
    glPushMatrix();

    MCFloat x = location().i();
    MCFloat y = location().j();

    if (pCamera) {
        pCamera->mapToCamera(x, y);
    }

    glTranslated(x, y, location().k());

    // Rotate
    if (angle() > 0) {
        glRotated(angle(), 0, 0, 1);
    }

    glBegin(GL_QUADS);

    // Scale alpha if fading out
    MCFloat alpha = m_pImpl->a;
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
        glColor4f(m_pImpl->r, m_pImpl->g, m_pImpl->b, alpha);
        glNormal3f(0, 0, 1.0f);
        glVertex2f(-r,  r);
        glVertex2f( r,  r);
        glVertex2f( r, -r);
        glVertex2f(-r, -r);
    }

    glEnd();

    glPopMatrix();
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

MCGLRectParticleGroup * MCGLRectParticle::group() const
{
    return m_pImpl->group;
}

void MCGLRectParticle::setGroup(MCGLRectParticleGroup * group)
{
    // Prevents rendering by MCWorld.
    setRenderable(group == nullptr);

    m_pImpl->group = group;
}

MCGLRectParticle::~MCGLRectParticle()
{
    delete m_pImpl;
}
