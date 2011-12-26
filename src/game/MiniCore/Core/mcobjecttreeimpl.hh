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

#ifndef MCOBJECTTREEIMPL_HH
#define MCOBJECTTREEIMPL_HH

//! Implementation class for MCObjectTree.
class MCObjectTreeImpl
{
    MCObjectTreeImpl(
        MCObjectTree * pPublic, MCFloat x1, MCFloat y1, MCFloat x2, MCFloat y2,
        MCUint leafMaxW, MCUint leafMaxH);
    ~MCObjectTreeImpl();

    void removeAll();
    void build();
    void insert(MCObject & object);
    bool remove(MCObject & object);
    void getIndexRange(const MCBBox<MCFloat> & bbox);
    void getBBoxCollisions(
        const MCObject & object, MCObjectTree::ObjectSet & resultObjs, MCUint typeId);
    void getObjectsWithinDistance(
        MCFloat x, MCFloat y, MCFloat d, MCObjectTree::ObjectSet & resultObjs,
        MCUint typeId);
    void getObjectsWithinBBox(
        const MCBBox<MCFloat> & rBBox, MCObjectTree::ObjectSet & resultObjs,
        MCUint typeId);
    MCObjectTree * const m_pPublic;
    MCBBox<MCFloat> m_bbox;
    MCUint m_leafMaxW, m_leafMaxH, m_horSize, m_verSize;
    MCUint m_i0, m_i1, m_j0, m_j1;
    MCFloat m_helpHor;
    MCFloat m_helpVer;
    MCObjectTree::ObjectSet * m_matrix;
    friend class MCObjectTree;
};

#endif // MCOBJECTTREEIMPL_HH
