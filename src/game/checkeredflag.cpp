// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "checkeredflag.hpp"

#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTextureManager"
#include "MiniCore/Core/MCTrigonom"

namespace
{
const int   ROWS             = 6;
const int   COLS             = 8;
const float AMPLITUDE        = 10.0f;
const float APPEARANCE_SPEED = 0.05f;
}

CheckeredFlag::CheckeredFlag()
{}

void CheckeredFlag::setDimensions(MCUint width, MCUint height)
{
    OverlayBase::setDimensions(width, height);

    const int nodeWidth  = this->width()  / COLS;
    const int nodeHeight = this->height() / ROWS;

    // Init the matrix
    m_matrix.clear();
    int angle = 0;
    for (int j = 0; j < ROWS; j++)
    {
        std::vector<CheckeredFlag::Node> row;
        for (int i = 0; i < COLS; i++)
        {
            CheckeredFlag::Node node;
            node.loc = MCVector3dF(this->width() / 2, this->height() * 2, 0);
            node.loc1.setI(i * nodeWidth  + nodeWidth  / 2);
            node.loc1.setJ(j * nodeHeight + nodeHeight / 2);
            node.loc1.setK(0);
            node.angle = ((i + j) * 40) % 360;
            row.push_back(node);
        }

        m_matrix.push_back(row);
    }
}

void CheckeredFlag::render()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glNormal3i(0, 0, 1);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);

    for (int j = 0; j < ROWS; j++)
    {
        for (int i = 0; i < COLS; i++)
        {
            CheckeredFlag::Node & node0 = m_matrix.at(j).at(i);

            // Assume 60 Hz update rate
            node0.angle += 8;
            if (node0.angle > 360)
            {
                node0.angle = 0;
            }

            node0.loc += (node0.loc1 - node0.loc) * APPEARANCE_SPEED;
            node0.loc.setK(MCTrigonom::sin(node0.angle) * AMPLITUDE);

            if ((j + i) & 0x1)
            {
                glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
            }
            else
            {
                glColor4f(0.0f, 0.0f, 0.0f, 0.25f);
            }

            if (i < COLS - 1 && j < ROWS - 1)
            {
                const CheckeredFlag::Node & node1 = m_matrix.at(j).at(i + 1);
                const CheckeredFlag::Node & node2 = m_matrix.at(j + 1).at(i + 1);
                const CheckeredFlag::Node & node3 = m_matrix.at(j + 1).at(i);

                glVertex3f(node0.loc.i(), node0.loc.j(), node0.loc.k());
                glVertex3f(node1.loc.i(), node1.loc.j(), node1.loc.k());
                glVertex3f(node2.loc.i(), node2.loc.j(), node2.loc.k());
                glVertex3f(node3.loc.i(), node3.loc.j(), node3.loc.k());
            }
        }
    }

    glEnd();
    glPopAttrib();
}
