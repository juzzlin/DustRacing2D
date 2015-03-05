// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcglshaderprogram.hh"
#include "mcsurfaceview.hh"
#include "mccamera.hh"
#include "mcsurface.hh"

MCSurfaceView::MCSurfaceView(
    const std::string & viewId,
    MCSurface * surface)
: MCShapeView(viewId)
, m_surface(surface)
{
    if (surface)
    {
        m_surface->setShaderProgram(shaderProgram());
        m_surface->setShadowShaderProgram(shadowShaderProgram());
    }
}

MCSurfaceView::~MCSurfaceView()
{}

void MCSurfaceView::setSurface(MCSurface & surface)
{
    m_surface = &surface;
    m_surface->setShaderProgram(shaderProgram());
    m_surface->setShadowShaderProgram(shadowShaderProgram());
}

MCSurface * MCSurfaceView::surface() const
{
    return m_surface;
}

void MCSurfaceView::setShaderProgram(MCGLShaderProgramPtr program)
{
    MCShapeView::setShaderProgram(program);
    m_surface->setShaderProgram(program);
}

void MCSurfaceView::setShadowShaderProgram(MCGLShaderProgramPtr program)
{
    MCShapeView::setShadowShaderProgram(program);
    m_surface->setShadowShaderProgram(program);
}

void MCSurfaceView::render(const MCVector3dF & l, MCFloat angle,
    MCCamera * p)
{
    m_surface->setScale(1.0f, 1.0f, 1.0f);
    m_surface->render(p, l, angle, !batchMode());
}

void MCSurfaceView::renderShadow(const MCVector3dF & l, MCFloat angle,
    MCCamera * p)
{
    m_surface->setScale(1.0f, 1.0f, 1.0f);
    m_surface->renderShadow(p, l, angle, !batchMode());
}

void MCSurfaceView::renderScaled(const MCVector3dF & l, MCFloat angle,
    MCFloat w, MCFloat h, MCCamera * p)
{
    m_surface->setSize(w, h);
    m_surface->render(p, l, angle, !batchMode());
}

void MCSurfaceView::renderShadowScaled(const MCVector3dF & l,
    MCFloat angle, MCFloat w, MCFloat h, MCCamera * p)
{
    m_surface->setSize(w, h);
    m_surface->renderShadow(p, l, angle, !batchMode());
}

void MCSurfaceView::beginBatch()
{
    m_surface->bind();
}

void MCSurfaceView::beginShadowBatch()
{
    m_surface->bindShadow();
}

void MCSurfaceView::endBatch()
{
    m_surface->release();
}

void MCSurfaceView::endShadowBatch()
{
    m_surface->releaseShadow();
}

MCBBox3dF MCSurfaceView::bbox() const
{
    // TODO: Fix this! The view should know the angle of the
    // shape somehow. Now we just return a naive bbox.

    const MCFloat w = m_surface->width() / 2;
    const MCFloat h = m_surface->height() / 2;
    const MCFloat r = std::max(w, h);

    return MCBBox3dF(-r, -r, m_surface->minZ(), r, r, m_surface->maxZ());
}

