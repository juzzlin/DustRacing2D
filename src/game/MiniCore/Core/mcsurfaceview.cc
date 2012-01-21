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

#include "mcsurfaceview.hh"
#include "mcsurfaceviewimpl.hh"
#include "mcsurface.hh"
#include "mccamera.hh"

MCSurfaceViewImpl::MCSurfaceViewImpl(MCSurface * pSurface) :
    m_pSurface(pSurface)
{}

MCSurfaceViewImpl::~MCSurfaceViewImpl()
{}

MCSurfaceView::MCSurfaceView(MCSurface * pSurface) :
    m_pImpl(new MCSurfaceViewImpl(pSurface))
{}

MCSurfaceView::~MCSurfaceView()
{
    delete m_pImpl;
}

void MCSurfaceView::setSurface(MCSurface * p)
{
    m_pImpl->m_pSurface = p;
}

MCSurface * MCSurfaceView::surface() const
{
    return m_pImpl->m_pSurface;
}

void MCSurfaceView::render(const MCVector3d<MCFloat> & l, MCUint angle,
    MCCamera * p)
{
    if (m_pImpl->m_pSurface) {
        m_pImpl->m_pSurface->render(p, l.i(), l.j(), l.k(), angle);
    }
}

void MCSurfaceView::renderShadow(const MCVector3d<MCFloat> & l, MCUint angle,
    MCCamera * p)
{
    if (m_pImpl->m_pSurface) {
        m_pImpl->m_pSurface->renderShadow(p, l.i(), l.j(), angle);
    }
}

void MCSurfaceView::renderScaled(const MCVector3d<MCFloat> & l, MCUint angle,
    MCFloat wr, MCFloat hr, MCCamera * p)
{
    if (m_pImpl->m_pSurface) {
        m_pImpl->m_pSurface->renderScaled(p, l.i(), l.j(), l.k(), wr, hr, angle);
    }
}

void MCSurfaceView::renderShadowScaled(const MCVector3d<MCFloat> & l,
    MCUint angle, MCFloat wr, MCFloat hr, MCCamera * p)
{
    if (m_pImpl->m_pSurface) {
        m_pImpl->m_pSurface->renderShadowScaled(p, l.i(), l.j(), wr, hr, angle);
    }
}

MCBBox<MCFloat> MCSurfaceView::bbox() const
{
    const MCFloat w = m_pImpl->m_pSurface->width() / 2;
    const MCFloat h = m_pImpl->m_pSurface->height() / 2;

    return MCBBox<MCFloat>(-w, -h, w, h);
}
