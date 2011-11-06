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

#include "mcglcircleparticle.hh"
#include "mcglcircleparticleimpl.hh"
#include "mccamera.hh"
#include "mctrigonom.hh"

#include <GL/gl.h>

MCRecycler<MCGLCircleParticle> MCGLCircleParticle::m_recycler;

vector<GLfloat> MCGLCircleParticleImpl::circleX;
vector<GLfloat> MCGLCircleParticleImpl::circleY;

namespace
{
  const MCUint FULL_CIRCLE  = 360;
  const MCUint SECTOR_ANGLE = 30;
}

MCGLCircleParticleImpl::MCGLCircleParticleImpl() :
  m_r(1.0f),
  m_g(1.0f),
  m_b(1.0f),
  m_a(1.0f)
{
  // Pre-calculate values
  if (!MCGLCircleParticleImpl::circleX.size())
  {
    for (MCUint i = 0; i <= FULL_CIRCLE; i += SECTOR_ANGLE)
    {
      MCGLCircleParticleImpl::circleX.push_back(MCTrigonom::cos(i));
      MCGLCircleParticleImpl::circleY.push_back(MCTrigonom::sin(i));
    }
  }
}

MCGLCircleParticleImpl::~MCGLCircleParticleImpl()
{}

MCGLCircleParticle::MCGLCircleParticle() :
    m_pImpl(new MCGLCircleParticleImpl)
{
  // Disable shadow by default
  setHasShadow(false);
}

void MCGLCircleParticle::setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a)
{
  m_pImpl->m_r = r;
  m_pImpl->m_g = g;
  m_pImpl->m_b = b;
  m_pImpl->m_a = a;
}

void MCGLCircleParticle::render(MCCamera * pCamera)
{
  MCFloat x = location().i();
  MCFloat y = location().j();
  MCFloat z = location().k();

  if (pCamera) {
    pCamera->mapToCamera(x, y);
  }

  // Disable texturing
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  glTranslated(x, y, z);

  // Rotate
  if (angle()) {
    glRotated(angle(), 0, 0, 1);
  }

  // Enable blending
  MCFloat alpha = m_pImpl->m_a;
  if (alpha < 1.0f || animationStyle() == FadeOut) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
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

  glColor4f(m_pImpl->m_r, m_pImpl->m_g, m_pImpl->m_b, alpha);
  glNormal3f(0, 0, 1.0);
  glScaled(r, r, 1);

  glBegin(GL_TRIANGLE_FAN);
  glVertex2i(0, 0);

  // Loop through the precalculated coordinates
  const MCUint i2 = MCGLCircleParticleImpl::circleX.size();
  for (MCUint i = 0; i < i2; i++)
  {
    glVertex2f(MCGLCircleParticleImpl::circleX[i], MCGLCircleParticleImpl::circleY[i]);
  }

  glEnd();
  glPopMatrix();
  glPopAttrib();
}

void MCGLCircleParticle::renderShadow(MCCamera *)
{
  return;
}

MCGLCircleParticle * MCGLCircleParticle::create()
{
  return MCGLCircleParticle::m_recycler.newObject();
}

void MCGLCircleParticle::recycle()
{
  MCGLCircleParticle::m_recycler.freeObject(this);
}

MCGLCircleParticle::~MCGLCircleParticle()
{
  delete m_pImpl;
}
