// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "treeview.hpp"

#include <MCCamera>
#include <MCSurface>
#include <MCTrigonom>

TreeView::TreeView(MCSurface & surface, MCFloat r0, MCFloat r1, MCFloat treeHeight, int branches)
: MCSurfaceView("TreeView", &surface)
, m_r0(r0)
, m_r1(r1)
, m_treeHeight(treeHeight)
, m_branches(branches)
, m_branchHeight(m_treeHeight / m_branches)
, m_dBranchHeight(m_branchHeight)
, m_dr((m_r1 - m_r0) / m_branches)
, m_dAngle(360 / (m_branches + 1))
, m_topSinAngle(0)
, m_topCosAngle(0)
{
}

TreeView::~TreeView()
{
}

void TreeView::render(const MCVector3d<MCFloat> & l, MCFloat, MCCamera * camera)
{
    m_top.setI(MCTrigonom::cos(m_topCosAngle));
    m_top.setJ(MCTrigonom::sin(m_topSinAngle / 2));

    int branchHeight = m_branchHeight;
    int r            = m_r0;
    int angle        = 0;
    int z            = branchHeight;

    const int amplitudeDivider = 12;
    for (int i = 0; i < m_branches; i++)
    {
        const MCFloat x = l.i() + m_top.i() * branchHeight / amplitudeDivider;
        const MCFloat y = l.j() + m_top.j() * branchHeight / amplitudeDivider;

        surface()->setSize(r * 2, r * 2);
        surface()->render(camera, MCVector3dF(x, y, z), angle, false);

        branchHeight += m_dBranchHeight;
        z            += m_dBranchHeight;
        r            += m_dr;
        angle        += m_dAngle;
    }
}

void TreeView::renderShadow(const MCVector3dF & l, MCFloat angle, MCCamera * p)
{
    surface()->setSize(m_r0 * 2, m_r0 * 2);
    surface()->renderShadow(p, l + MCVector3dF(5, -5, 1), angle, false);
}

void TreeView::beginBatch()
{
    surface()->bind();
}

void TreeView::endBatch()
{
}

void TreeView::beginShadowBatch()
{
    surface()->bindShadow();
}

void TreeView::endShadowBatch()
{
}

bool TreeView::update()
{
    if (++m_topCosAngle >= 360)
    {
        m_topCosAngle = 0;
    }

    if (++m_topSinAngle >= 720)
    {
        m_topSinAngle = 0;
    }

    return true;
}

void TreeView::reset()
{
}
