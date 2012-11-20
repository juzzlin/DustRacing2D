// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef TREEVIEW_HPP
#define TREEVIEW_HPP

#include <MCSurfaceView>
#include "updateableif.hpp"

/*! Custom view used to render tree objects by using a
 *  couple of MCSurfaces.
 */
class TreeView : public MCSurfaceView, public UpdateableIf
{
public:

    /*! Constructor.
     * \param pSurface Surface to be used. Ownership of pSurface is not changed.
     */
    TreeView(MCSurface & surface, MCFloat r0, MCFloat r1, MCFloat treeHeight, int branches);

    //! Destructor
    virtual ~TreeView();

    //! \reimp
    virtual void render(const MCVector3dF & l, MCFloat angle,
        MCCamera * p = nullptr);

    //! \reimp
    virtual void renderShadow(const MCVector3dF & l, MCFloat angle,
        MCCamera * p = nullptr);

    //! \reimp
    virtual void beginBatch();

    //! \reimp
    virtual void endBatch();

    //! \reimp
    virtual void beginShadowBatch();

    //! \reimp
    virtual void endShadowBatch();

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

private:

    int m_r0, m_r1, m_treeHeight;
    int m_branches;
    int m_branchHeight, m_dBranchHeight;
    int m_dr;
    int m_dAngle;
    MCUint m_topSinAngle;
    MCUint m_topCosAngle;
    MCVector2dF m_top;
};

#endif // TREEVIEW_HPP
