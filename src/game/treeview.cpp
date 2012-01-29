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
, m_topAngle(0)
{
}

TreeView::~TreeView()
{
}

void TreeView::render(const MCVector3d<MCFloat> & l, MCUint, MCCamera * pCamera)
{
    if (++m_topAngle >= 720)
    {
        m_topAngle = 0;
    }

    m_top.setI(MCTrigonom::cos(m_topAngle / 2));
    m_top.setJ(MCTrigonom::sin(m_topAngle / 2));

    MCFloat branchHeight = m_branchHeight;
    MCFloat r            = m_r0;
    MCUint  angle        = 0;

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, surface()->handle());

    surface()->doAlphaTest();

    for (int i = 0; i < m_branches; i++)
    {
        MCFloat x = l.i() + m_top.i() * branchHeight / 2;
        MCFloat y = l.j() + m_top.j() * branchHeight / 2;

        const MCFloat z = branchHeight;

        if (pCamera)
        {
            pCamera->mapToCamera(x, y);
        }

        glPushMatrix();
        glTranslated(x, y, z);
        glRotated(angle, 0, 0, 1);

        glNormal3i(0, 0, 1);

        glBegin(GL_QUADS);

        glTexCoord2i(0, 0);
        glVertex2f(-r, -r);
        glTexCoord2i(0, 1);
        glVertex2f(-r, r);
        glTexCoord2i(1, 1);
        glVertex2f(r, r);
        glTexCoord2i(1, 0);
        glVertex2f(r, -r);

        glEnd();

        glPopMatrix();

        branchHeight += m_dBranchHeight;
        r            += m_dr;
        angle        += m_dAngle;
    }

    glPopAttrib();
}

void TreeView::renderShadow(const MCVector3d<MCFloat> & l, MCUint angle, MCCamera * p)
{
    surface()->renderShadowScaled(p, l.i(), l.j(), m_r0, m_r0, angle);
}
