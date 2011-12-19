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

#include "mcsurface.hh"
#include "mcsurfaceimpl.hh"
#include "mccamera.hh"
#include "mcbbox.hh"
#include "mctrigonom.hh"

MCSurfaceImpl::MCSurfaceImpl(
    GLuint newHandle, MCFloat newWidth, MCFloat newHeight)
: m_handle(newHandle)
, m_w(newWidth)
, m_w2(newWidth / 2)
, m_h(newHeight)
, m_h2(newHeight / 2)
, m_center(m_w2, m_h2)
, m_centerSet(false)
, m_useAlphaTest(false)
, m_alphaFunc(GL_ALWAYS)
, m_alphaThreshold(0.0f)
{}

MCSurface::MCSurface(GLuint newHandle, MCFloat newWidth, MCFloat newHeight)
: m_pImpl(new MCSurfaceImpl(newHandle, newWidth, newHeight))
{}

MCSurface::~MCSurface()
{
    delete m_pImpl;
}

void MCSurface::setCenter(const MCVector2d<MCFloat> & center)
{
    m_pImpl->m_centerSet = true;
    m_pImpl->m_center    = center;
}

void MCSurface::setAlphaTest(
    bool useAlphaTest, GLenum alphaFunc, GLclampf threshold)
{
    m_pImpl->m_useAlphaTest   = useAlphaTest;
    m_pImpl->m_alphaFunc      = alphaFunc;
    m_pImpl->m_alphaThreshold = threshold;
}

MCBBox<MCFloat> MCSurface::rotatedBBox(MCFloat x, MCFloat y, int angle)
{
    return m_pImpl->rotatedBBox(x, y, angle);
}

MCBBox<MCFloat> MCSurfaceImpl::rotatedBBox(MCFloat x, MCFloat y, int angle)
{
    using std::abs;

    const MCFloat cos = MCTrigonom::cos(angle);
    const MCFloat sin = MCTrigonom::sin(angle);
    const MCFloat  w1 = (abs(cos * m_w2) + abs(sin * m_h2)) / 2;
    const MCFloat  h1 = (abs(sin * m_w2) + abs(cos * m_h2)) / 2;

    return MCBBox<MCFloat>(x - w1, y - h1, x + w1, y + h1);
}

MCBBox<MCFloat> MCSurface::rotatedScaledBBox(
    MCFloat x, MCFloat y, int angle, MCFloat w2, MCFloat h2)
{
    return m_pImpl->rotatedScaledBBox(x, y, angle, w2, h2);
}

MCBBox<MCFloat> MCSurfaceImpl::rotatedScaledBBox(
    MCFloat x, MCFloat y, int angle, MCFloat w2, MCFloat h2)
{
    using std::abs;

    const MCFloat cos = MCTrigonom::cos(angle);
    const MCFloat sin = MCTrigonom::sin(angle);
    const MCFloat  w1 = (abs(cos * w2) + abs(sin * h2)) / 2;
    const MCFloat  h1 = (abs(sin * w2) + abs(cos * h2)) / 2;

    return MCBBox<MCFloat>(x - w1, y - h1, x + w1, y + h1);
}

void MCSurface::render(
    MCCamera * pCamera, MCFloat x, MCFloat y, MCFloat z, int angle)
{
    m_pImpl->render(pCamera, x, y, z, angle);
}

void MCSurfaceImpl::render(
    MCCamera * pCamera, MCFloat x, MCFloat y, MCFloat z, int angle)
{
    if (pCamera)
    {
        pCamera->mapToCamera(x, y);
    }

    glPushAttrib(GL_ENABLE_BIT);
    glPushMatrix();
    glTranslated(x, y, z);

    if (angle)
    {
        glRotated(angle, 0, 0, 1);
    }

    if (m_centerSet)
    {
        glTranslated(m_w2 - m_center.i(), m_h2 - m_center.j(), z);
    }

    doAlphaTest();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glNormal3i(0, 0, 1);

    glBegin(GL_QUADS);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);

    glTexCoord2i(0, 0);
    glVertex3f(-m_w2, -m_h2, 0);
    glTexCoord2i(0, 1);
    glVertex3f(-m_w2, m_h2, 0);
    glTexCoord2i(1, 1);
    glVertex3f(m_w2, m_h2, 0);
    glTexCoord2i(1, 0);
    glVertex3f(m_w2, -m_h2, 0);
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

void MCSurface::renderScaled(
    MCCamera * pCamera,
    MCFloat x, MCFloat y, MCFloat z, MCFloat wr, MCFloat hr, int angle)
{
    m_pImpl->renderScaled(pCamera, x, y, z, wr, hr, angle);
}

void MCSurfaceImpl::renderScaled(
    MCCamera * pCamera,
    MCFloat x, MCFloat y, MCFloat z, MCFloat wr, MCFloat hr, int angle)
{
    if (pCamera)
    {
        pCamera->mapToCamera(x, y);
    }

    glPushAttrib(GL_ENABLE_BIT);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslated(x, y, z);

    if (angle)
    {
        glRotated(angle, 0, 0, 1);
    }

    if (m_centerSet)
    {
        glTranslated(m_w2 - m_center.i(), m_h2 - m_center.j(), z);
    }

    doAlphaTest();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glNormal3i(0, 0, 1);

    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2f(-wr, -hr);
    glTexCoord2i(0, 1);
    glVertex2f(-wr, hr);
    glTexCoord2i(1, 1);
    glVertex2f(wr, hr);
    glTexCoord2i(1, 0);
    glVertex2f(wr, -hr);
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

void MCSurface::renderShadow(
    MCCamera * pCamera, MCFloat x, MCFloat y, int angle)
{
    m_pImpl->renderShadow(pCamera, x, y, angle);
}

void MCSurfaceImpl::renderShadow(
    MCCamera * pCamera, MCFloat x, MCFloat y, int angle)
{
    if (pCamera)
    {
        pCamera->mapToCamera(x, y);
    }

    glPushAttrib(GL_ENABLE_BIT);
    glPushMatrix();
    glTranslated(x, y, 0);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    if (angle)
    {
        glRotated(angle, 0, 0, 1);
    }

    if (m_centerSet)
    {
        glTranslated(m_w2 - m_center.i(), m_h2 - m_center.j(), 0);
    }

    doAlphaTest();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_ZERO);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glNormal3i(0, 0, 1);

    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2f(-m_w2, -m_h2);
    glTexCoord2i(0, 1);
    glVertex2f(-m_w2, m_h2);
    glTexCoord2i(1, 1);
    glVertex2f(m_w2, m_h2);
    glTexCoord2i(1, 0);
    glVertex2f(m_w2, -m_h2);
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

void MCSurface::renderShadowScaled(
    MCCamera * pCamera,
    MCFloat x, MCFloat y, MCFloat wr, MCFloat hr, int angle)
{
    m_pImpl->renderShadowScaled(pCamera, x, y, wr, hr, angle);
}

void MCSurfaceImpl::renderShadowScaled(
    MCCamera * pCamera,
    MCFloat x, MCFloat y, MCFloat wr, MCFloat hr, int angle)
{
    if (pCamera)
    {
        pCamera->mapToCamera(x, y);
    }

    glPushAttrib(GL_ENABLE_BIT);
    glPushMatrix();
    glTranslated(x, y, 0);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    if (angle)
    {
        glRotated(angle, 0, 0, 1);
    }

    if (m_centerSet)
    {
        glTranslated(m_w2 - m_center.i(), m_h2 - m_center.j(), 0);
    }

    doAlphaTest();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_ZERO);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glNormal3i(0, 0, 1);

    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2f(-wr, -hr);
    glTexCoord2i(0, 1);
    glVertex2f(-wr, hr);
    glTexCoord2i(1, 1);
    glVertex2f(wr, hr);
    glTexCoord2i(1, 0);
    glVertex2f(wr, -hr);
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

void MCSurfaceImpl::doAlphaTest() const
{
    if (m_useAlphaTest)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(m_alphaFunc, m_alphaThreshold);
    }
}

GLuint MCSurface::handle() const
{
    return m_pImpl->m_handle;
}

MCFloat MCSurface::width() const
{
    return m_pImpl->m_w;
}

MCFloat MCSurface::height() const
{
    return m_pImpl->m_h;
}

MCVector2d<MCFloat> MCSurface::center() const
{
    return m_pImpl->m_center;
}
