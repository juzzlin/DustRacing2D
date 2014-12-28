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

#include "mcobjectgrid.hh"

#include <algorithm>

MCObjectGrid::MCObjectGrid(
    MCFloat x1, MCFloat y1, MCFloat x2, MCFloat y2,
    MCFloat leafMaxW, MCFloat leafMaxH)
: m_bbox(x1, y1, x2, y2)
, m_leafMaxW(leafMaxW)
, m_leafMaxH(leafMaxH)
, m_horSize((x2 - x1) / m_leafMaxW)
, m_verSize((y2 - y1) / m_leafMaxH)
, m_helpHor(static_cast<MCFloat>(m_horSize) / (x2 - x1))
, m_helpVer(static_cast<MCFloat>(m_verSize) / (y2 - y1))
{
    build();
}

MCObjectGrid::~MCObjectGrid()
{
    delete [] m_matrix;
}

void MCObjectGrid::setIndexRange(const MCBBox<MCFloat> & bbox)
{
    int temp = static_cast<int>(bbox.x1() * m_helpHor);
    if (temp >= static_cast<int>(m_horSize)) temp = m_horSize - 1;
    else if (temp < 0) temp = 0;
    m_i0 = static_cast<MCUint>(temp);

    temp = static_cast<int>(bbox.x2() * m_helpHor);
    if (temp >= static_cast<int>(m_horSize)) temp = m_horSize - 1;
    else if (temp < 0) temp = 0;
    m_i1 = static_cast<MCUint>(temp);

    temp = static_cast<int>(bbox.y1() * m_helpVer);
    if (temp >= static_cast<int>(m_verSize)) temp = m_verSize - 1;
    else if (temp < 0) temp = 0;
    m_j0 = static_cast<MCUint>(temp);

    temp = static_cast<int>(bbox.y2() * m_helpVer);
    if (temp >= static_cast<int>(m_verSize)) temp = m_verSize - 1;
    else if (temp < 0) temp = 0;
    m_j1 = static_cast<MCUint>(temp);
}

void MCObjectGrid::insert(MCObject & object)
{
    setIndexRange(object.bbox());
    object.cacheIndexRange(m_i0, m_i1, m_j0, m_j1);

    for (MCUint j = m_j0; j <= m_j1; j++)
    {
        for (MCUint i = m_i0; i <= m_i1; i++)
        {
            const int index = j * m_horSize + i;
            GridCell & cell = m_matrix[index];
            cell.m_objects.insert(&object);
            m_dirtyCellCache.insert(&cell);
        }
    }
}

bool MCObjectGrid::remove(MCObject & object)
{
    bool removed = false;
    object.restoreIndexRange(&m_i0, &m_i1, &m_j0, &m_j1);

    for (MCUint j = m_j0; j <= m_j1; j++)
    {
        for (MCUint i = m_i0; i <= m_i1; i++)
        {
            const int index = j * m_horSize + i;
            GridCell & cell = m_matrix[index];
            const auto iter(cell.m_objects.find(&object));
            const auto end(cell.m_objects.end());
            if (iter != end)
            {
                cell.m_objects.erase(iter);
                removed = true;

                if (!cell.m_objects.size())
                {
                    m_dirtyCellCache.erase(&cell);
                }
            }
        }
    }
    return removed;
}

void MCObjectGrid::removeAll()
{
    for (MCUint j = 0; j < m_verSize; j++)
    {
        for (MCUint i = 0; i < m_horSize; i++)
        {
            const int index = j * m_horSize + i;
            m_matrix[index].m_objects.clear();
        }
    }

    m_dirtyCellCache.clear();
}

void MCObjectGrid::build()
{
    m_matrix = new MCObjectGrid::GridCell[m_horSize * m_verSize];
    for (MCUint j = 0; j < m_verSize; j++)
    {
        for (MCUint i = 0; i < m_horSize; i++)
        {
            m_matrix[j * m_horSize + i].m_objects = MCObjectGrid::ObjectSet();
        }
    }
}

void MCObjectGrid::getBBoxCollisions(MCObjectGrid::CollisionVector & result)
{
    result.clear();

    // Optimization: ignore collisions between sleeping objects.
    // Note that stationary objects are also sleeping objects.

    auto cellIter = m_dirtyCellCache.begin();
    while (cellIter != m_dirtyCellCache.end())
    {
        bool hadCollisions = false;
        const MCObjectGrid::ObjectSet & objects = (*cellIter)->m_objects;

        auto outer(objects.begin());
        const auto end(objects.end());
        while (outer != end)
        {
            MCObject * obj1 = *outer;
            auto inner = objects.begin();
            while (inner != end)
            {
                MCObject * obj2 = *inner;
                if (obj1 != obj2 &&
                    &obj1->parent() != obj2 &&
                    &obj2->parent() != obj1 &&
                    (!obj1->sleeping() || !obj2->sleeping()) &&
                    (obj1->collisionLayer() == obj2->collisionLayer() || obj1->collisionLayer() == -1) &&
                    (obj1->bbox().intersects(obj2->bbox())))
                {
                    result[obj1].insert(obj2);
                    hadCollisions = true;
                }

                inner++;
            }

            outer++;
        }

        if (!hadCollisions)
        {
            cellIter = m_dirtyCellCache.erase(cellIter);
        }
        else
        {
            cellIter++;
        }
    }
}

void MCObjectGrid::getObjectsWithinDistance(
    MCFloat x, MCFloat y, MCFloat d,
    MCObjectGrid::ObjectSet & resultObjs)
{
    setIndexRange(MCBBox<MCFloat>(x - d, y - d, x + d, y + d));

    // Pre-square the distance
    d *= d;

    resultObjs.clear();
    MCObjectGrid::ObjectSet::iterator iter;
    MCObject * p = nullptr;

    for (MCUint j = m_j0; j <= m_j1; j++)
    {
        for (MCUint i = m_i0; i <= m_i1; i++)
        {
            const int index = j * m_horSize + i;
            const auto end(m_matrix[index].m_objects.end());
            iter = m_matrix[index].m_objects.begin();
            while (iter != end)
            {
                p = *iter;
                const MCFloat x2 = x - p->location().i();
                const MCFloat y2 = y - p->location().j();

                if (x2 * x2 + y2 * y2 < d)
                {
                    resultObjs.insert(p);
                }

                iter++;
            }
        }
    }
}

void MCObjectGrid::getObjectsWithinBBox(
    const MCBBox<MCFloat> & bbox,
    MCObjectGrid::ObjectSet & resultObjs)
{
    setIndexRange(bbox);

    resultObjs.clear();
    MCObjectGrid::ObjectSet::iterator iter;
    MCObject * p = nullptr;

    for (MCUint j = m_j0; j <= m_j1; j++)
    {
        for (MCUint i = m_i0; i <= m_i1; i++)
        {
            const int index = j * m_horSize + i;
            iter = m_matrix[index].m_objects.begin();
            while (iter != m_matrix[index].m_objects.end())
            {
                p = *iter;
                if (bbox.intersects(p->bbox()))
                {
                    resultObjs.insert(p);
                }

                iter++;
            }
        }
    }
}

const MCBBox<MCFloat> & MCObjectGrid::bbox() const
{
    return m_bbox;
}
