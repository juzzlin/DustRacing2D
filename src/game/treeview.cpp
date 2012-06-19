// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "treeview.hpp"

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTrigonom"

TreeView::TreeView(MCSurface & surface, MCFloat r0, MCFloat r1, MCFloat treeHeight, int branches)
: MCSurfaceView(&surface)
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

void TreeView::render(const MCVector3d<MCFloat> & l, MCFloat, MCCamera * pCamera)
{
    if (++m_topCosAngle >= 360)
    {
        m_topCosAngle = 0;
    }

    if (++m_topSinAngle >= 720)
    {
        m_topSinAngle = 0;
    }

    m_top.setI(MCTrigonom::cos(m_topCosAngle));
    m_top.setJ(MCTrigonom::sin(m_topSinAngle / 2));

    MCFloat branchHeight = m_branchHeight;
    MCFloat r            = m_r0;
    MCUint  angle        = 0;

    surface()->enableClientState(true);

    MCFloat z = branchHeight / 3;
    for (int i = 0; i < m_branches; i++)
    {
        MCFloat x = l.i() + m_top.i() * branchHeight / 4;
        MCFloat y = l.j() + m_top.j() * branchHeight / 4;

        surface()->renderScaled(pCamera, MCVector3dF(x, y, z), r, r, angle, false);

        branchHeight += m_dBranchHeight;
        z            += m_dBranchHeight;
        r            += m_dr;
        angle        += m_dAngle;
    }

    surface()->enableClientState(false);
}

void TreeView::renderShadow(const MCVector3d<MCFloat> & l, MCFloat angle, MCCamera * p)
{
    surface()->renderShadowScaled(p, l, m_r0, m_r0, angle);
}
