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

#ifndef TREEVIEW_HPP
#define TREEVIEW_HPP

#include "MiniCore/Core/MCSurfaceView"

//! Custom view used to render tree objects.
class TreeView : public MCSurfaceView
{
public:

    /*! Constructor.
     * \param pSurface Surface to be used. Ownership of pSurface is not changed.
     */
    TreeView(MCSurface & surface, MCFloat r0, MCFloat r1, MCFloat treeHeight, int branches);

    //! Destructor
    virtual ~TreeView();

    //! \reimp
    virtual void render(const MCVector3d<MCFloat> & l, MCUint angle,
        MCCamera * p = nullptr);

    //! \reimp
    virtual void renderShadow(const MCVector3d<MCFloat> & l, MCUint angle,
        MCCamera * p = nullptr);

private:

    MCFloat m_r0, m_r1, m_treeHeight;
    int m_branches;
    MCFloat m_branchHeight, m_dBranchHeight;
    MCFloat m_dr;
    MCFloat m_dAngle;
    MCUint m_topSinAngle;
    MCUint m_topCosAngle;
    MCVector2d<MCFloat> m_top;
};

#endif // TREEVIEW_HPP
