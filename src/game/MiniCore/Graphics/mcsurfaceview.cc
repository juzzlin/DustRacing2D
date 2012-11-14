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

#include "mcglshaderprogram.hh"
#include "mcsurfaceview.hh"
#include "mccamera.hh"
#include "mcsurface.hh"

MCSurfaceView::MCSurfaceView(
    const std::string & viewId,
    MCSurface * surface,
    bool batchMode)
: MCShapeView(viewId)
, m_surface(surface)
, m_batchMode(batchMode)
{}

MCSurfaceView::~MCSurfaceView()
{}

void MCSurfaceView::setSurface(MCSurface & surface)
{
    m_surface = &surface;
}

MCSurface * MCSurfaceView::surface() const
{
    return m_surface;
}

void MCSurfaceView::setShaderProgram(MCGLShaderProgram * program)
{
    if (m_surface)
    {
        m_surface->setShaderProgram(program);
    }
}

void MCSurfaceView::setShadowShaderProgram(MCGLShaderProgram * program)
{
    if (m_surface)
    {
        m_surface->setShadowShaderProgram(program);
    }
}

void MCSurfaceView::render(const MCVector3d<MCFloat> & l, MCFloat angle,
    MCCamera * p)
{
    if (m_surface)
    {
        m_surface->render(p, l, angle, !m_batchMode);
    }
}

void MCSurfaceView::renderShadow(const MCVector3d<MCFloat> & l, MCFloat angle,
    MCCamera * p)
{
    if (m_surface)
    {
        m_surface->renderShadow(p, l, angle, !m_batchMode);
    }
}

void MCSurfaceView::renderScaled(const MCVector3d<MCFloat> & l, MCFloat angle,
    MCFloat wr, MCFloat hr, MCCamera * p)
{
    if (m_surface)
    {
        m_surface->renderScaled(p, l, wr, hr, angle, !m_batchMode);
    }
}

void MCSurfaceView::renderShadowScaled(const MCVector3d<MCFloat> & l,
    MCFloat angle, MCFloat wr, MCFloat hr, MCCamera * p)
{
    if (m_surface)
    {
        m_surface->renderShadowScaled(p, l, wr, hr, angle, !m_batchMode);
    }
}

void MCSurfaceView::beginBatch()
{
    if (m_batchMode && m_surface)
    {
        m_surface->enableClientState(true);
    }
}

void MCSurfaceView::endBatch()
{
    if (m_batchMode && m_surface)
    {
        m_surface->enableClientState(false);
    }
}

MCBBox<MCFloat> MCSurfaceView::bbox() const
{
    // TODO: Fix this! The view should know the angle of the
    // shape somehow. Now we just return a naive bbox.

    const MCFloat w = m_surface->width() / 2;
    const MCFloat h = m_surface->height() / 2;
    const MCFloat r = std::max(w, h);

    return MCBBox<MCFloat>(-r, -r, r, r);
}

