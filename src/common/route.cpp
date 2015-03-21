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

#include "route.hpp"
#include "targetnodebase.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace {
static const int CLOSING_TH = 32;
}

Route::Route()
{
}

void Route::clear()
{
    m_route.clear();
}

bool Route::push(TargetNodePtr tnode)
{
    tnode->setIndex(static_cast<int>(m_route.size()));
    m_route.push_back(tnode);
    return isClosed();
}

bool Route::isClosed() const
{
    if (m_route.size() > 1)
    {
        const int dx = std::abs(m_route[0]->location().x() - m_route.back()->location().x());
        const int dy = std::abs(m_route[0]->location().y() - m_route.back()->location().y());

        if (dx < CLOSING_TH && dy < CLOSING_TH)
        {
            return true;
        }
    }

    return false;
}

unsigned int Route::numNodes() const
{
    return static_cast<unsigned int>(m_route.size());
}

TargetNodePtr Route::get(unsigned int index) const
{
    assert (index < numNodes());
    return m_route[index];
}

void Route::getAll(std::vector<TargetNodePtr> & routeVector) const
{
    routeVector = m_route;
}

void Route::buildFromVector(std::vector<TargetNodePtr> & routeVector)
{
    clear();

    std::sort(routeVector.begin(), routeVector.end(),
        [] (const TargetNodePtr lhs, const TargetNodePtr rhs)
        {
            return lhs->index() < rhs->index();
        });

    for (TargetNodePtr tnode : routeVector)
    {
        if (tnode && tnode->index() >= 0)
        {
            push(tnode);
        }
    }
}

unsigned int Route::geometricLength() const
{
    unsigned int result = 0;

    if (m_route.size() > 1)
    {
        for (unsigned int i = 0; i < m_route.size() - 1; i++)
        {
            int dx = m_route[i]->location().x() - m_route[i + 1]->location().x();
            int dy = m_route[i]->location().y() - m_route[i + 1]->location().y();
            result += std::sqrt(dx * dx + dy * dy);
        }

        int dx = m_route[m_route.size() - 1]->location().x() - m_route[0]->location().x();
        int dy = m_route[m_route.size() - 1]->location().y() - m_route[0]->location().y();
        result += std::sqrt(dx * dx + dy * dy);
    }

    return result;
}
