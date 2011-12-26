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

#include "mcobjecttree.hh"
#include "mcobjecttreeimpl.hh"

#include <algorithm>

MCObjectTreeImpl::MCObjectTreeImpl(MCObjectTree * pPublic,
                               MCFloat x1, MCFloat y1, MCFloat x2, MCFloat y2,
                               MCUint leafMaxW, MCUint leafMaxH)
: m_pPublic(pPublic)
, m_bbox(x1, y1, x2, y2)
, m_leafMaxW(leafMaxW)
, m_leafMaxH(leafMaxH)
, m_horSize((x2 - x1) / m_leafMaxW)
, m_verSize((y2 - y1) / m_leafMaxH)
, m_helpHor(static_cast<MCFloat>(m_horSize) / (x2 - x1))
, m_helpVer(static_cast<MCFloat>(m_verSize) / (y2 - y1))
{
    build();
}

MCObjectTreeImpl::~MCObjectTreeImpl()
{
    delete [] m_matrix;
}

MCObjectTree::MCObjectTree(MCFloat x1, MCFloat y1, MCFloat x2, MCFloat y2,
    MCUint leafMaxW, MCUint leafMaxH)
: m_pImpl(new MCObjectTreeImpl(this, x1, y1, x2, y2, leafMaxW, leafMaxH))
{}

MCObjectTree::~MCObjectTree()
{
    delete m_pImpl;
}

void MCObjectTreeImpl::getIndexRange(const MCBBox<MCFloat> & rBBox)
{
    int temp = static_cast<int>(rBBox.x1() * m_helpHor);
    if (temp >= static_cast<int>(m_horSize)) temp = m_horSize - 1;
    else if (temp < 0) temp = 0;
    m_i0 = static_cast<MCUint>(temp);

    temp = static_cast<int>(rBBox.x2() * m_helpHor);
    if (temp >= static_cast<int>(m_horSize)) temp = m_horSize - 1;
    else if (temp < 0) temp = 0;
    m_i1 = static_cast<MCUint>(temp);

    temp = static_cast<int>(rBBox.y1() * m_helpVer);
    if (temp >= static_cast<int>(m_verSize)) temp = m_verSize - 1;
    else if (temp < 0) temp = 0;
    m_j0 = static_cast<MCUint>(temp);

    temp = static_cast<int>(rBBox.y2() * m_helpVer);
    if (temp >= static_cast<int>(m_verSize)) temp = m_verSize - 1;
    else if (temp < 0) temp = 0;
    m_j1 = static_cast<MCUint>(temp);
}

void MCObjectTree::insert(MCObject & object)
{
    m_pImpl->insert(object);
}

void MCObjectTreeImpl::insert(MCObject & object)
{
    getIndexRange(object.bbox());
    object.cacheIndexRange(m_i0, m_i1, m_j0, m_j1);
    for (MCUint j = m_j0; j <= m_j1; j++) {
        for (MCUint i = m_i0; i <= m_i1; i++) {
            m_matrix[j * m_horSize + i].insert(&object);
        }
    }
}

bool MCObjectTree::remove(MCObject & object)
{
    return m_pImpl->remove(object);
}

bool MCObjectTreeImpl::remove(MCObject & object)
{
    bool removed = false;
    object.restoreIndexRange(&m_i0, &m_i1, &m_j0, &m_j1);
    for (MCUint j = m_j0; j <= m_j1; j++) {
        for (MCUint i = m_i0; i <= m_i1; i++) {
            const int index = j * m_horSize + i;
            MCObjectTree::ObjectSet::iterator iter(m_matrix[index].find(&object));
            if (iter != m_matrix[index].end()) {
                m_matrix[index].erase(iter);
                removed = true;
            }
        }
    }
    return removed;
}

void MCObjectTree::removeAll()
{
    m_pImpl->removeAll();
}

void MCObjectTreeImpl::removeAll()
{
    for (MCUint j = 0; j < m_verSize; j++) {
        for (MCUint i = 0; i < m_horSize; i++) {
            m_matrix[j * m_horSize + i].clear();
        }
    }
}

void MCObjectTreeImpl::build()
{
    m_matrix = new MCObjectTree::ObjectSet[m_horSize * m_verSize];
    for (MCUint j = 0; j < m_verSize; j++) {
        for (MCUint i = 0; i < m_horSize; i++) {
            m_matrix[j * m_horSize + i] = MCObjectTree::ObjectSet();
        }
    }
}

void MCObjectTree::getBBoxCollisions(
    const MCObject & object, MCObjectTree::ObjectSet & resultObjs, MCUint typeId)
{
    m_pImpl->getBBoxCollisions(object, resultObjs, typeId);
}

void MCObjectTreeImpl::getBBoxCollisions(
    const MCObject & object, MCObjectTree::ObjectSet & resultObjs, MCUint typeId)
{
    resultObjs.clear();

    const MCBBox<MCFloat> b(object.bbox());
    getIndexRange(b);
    MCObject * p2 = nullptr;
    MCObjectTree::ObjectSet::iterator iter;

    for (MCUint j = m_j0; j <= m_j1; j++) {
        for (MCUint i = m_i0; i <= m_i1; i++) {
            const int index = j * m_horSize + i;
            iter = m_matrix[index].begin();
            while (iter != m_matrix[index].end()) {
                p2 = *iter;
                if ((&object != p2) && (!typeId || p2->typeID() == typeId)) {
                    if (b.intersects(p2->bbox())) {
                        resultObjs.insert(p2);
                    }
                }
                iter++;
            }
        }
    }
}

void MCObjectTree::getObjectsWithinDistance(
    MCFloat x, MCFloat y, MCFloat d,
    MCObjectTree::ObjectSet & resultObjs,
    MCUint typeId)
{
    m_pImpl->getObjectsWithinDistance(x, y, d, resultObjs, typeId);
}

void MCObjectTreeImpl::getObjectsWithinDistance(
    MCFloat x, MCFloat y, MCFloat d,
    MCObjectTree::ObjectSet & resultObjs,
    MCUint typeId)
{
    getIndexRange(MCBBox<MCFloat>(x - d, y - d, x + d, y + d));

    // Pre-square the distance
    d *= d;

    resultObjs.clear();
    MCObjectTree::ObjectSet::iterator iter;
    MCObject * p = nullptr;

    for (MCUint j = m_j0; j <= m_j1; j++) {
        for (MCUint i = m_i0; i <= m_i1; i++) {
            const int index = j * m_horSize + i;
            iter = m_matrix[index].begin();
            while (iter != m_matrix[index].end()) {
                p = *iter;
                if (!typeId || p->typeID() == typeId) {

                    const MCFloat x2 = x - p->getX();
                    const MCFloat y2 = y - p->getY();

                    if (x2 * x2 + y2 * y2 < d) {
                        resultObjs.insert(p);
                    }
                }

                iter++;
            }
        }
    }
}

void MCObjectTree::getObjectsWithinBBox(
    const MCBBox<MCFloat> & rBBox,
    MCObjectTree::ObjectSet & resultObjs,
    MCUint typeId)
{
    m_pImpl->getObjectsWithinBBox(rBBox, resultObjs, typeId);
}

void MCObjectTreeImpl::getObjectsWithinBBox(
    const MCBBox<MCFloat> & rBBox,
    MCObjectTree::ObjectSet & resultObjs,
    MCUint typeId)
{
    getIndexRange(rBBox);

    resultObjs.clear();
    MCObjectTree::ObjectSet::iterator iter;
    MCObject * p = nullptr;

    for (MCUint j = m_j0; j <= m_j1; j++) {
        for (MCUint i = m_i0; i <= m_i1; i++) {
            const int index = j * m_horSize + i;
            iter = m_matrix[index].begin();
            while (iter != m_matrix[index].end()) {
                p = *iter;
                if (!typeId || p->typeID() == typeId) {
                    if (rBBox.intersects(p->bbox())) {
                        resultObjs.insert(p);
                    }
                }

                iter++;
            }
        }
    }
}

const MCBBox<MCFloat> & MCObjectTree::bbox() const
{
    return m_pImpl->m_bbox;
}
