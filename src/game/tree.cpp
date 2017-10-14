// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#include "tree.hpp"

#include <MCSurface>
#include <MCPhysicsComponent>
#include <MCCircleShape>
#include <MCRandom>
#include <MCShape>
#include <MCShapeView>

namespace {
static const MCFloat treeBodyRadius = 8;
static const MCFloat treeViewRadius = 48;
}

Tree::Tree(MCSurface & surface, MCFloat r0, MCFloat r1, MCFloat treeHeight, int branches)
    : MCObject(MCShapePtr(new MCCircleShape(nullptr, treeBodyRadius)), "tree")
{
    physicsComponent().setMass(1, true); // Stationary
    physicsComponent().setRestitution(0.25f);

    setIsPhysicsObject(true);

    shape()->setRadius(treeBodyRadius);

    const float branchHeight = treeHeight / branches;
    for (int i = 0; i < branches; i++)
    {
        auto branch = new MCObject(surface, i == 0 ? "treeRoot" : "treeBranch");

        if (i == 0)
        {
            branch->shape()->view()->setHasShadow(true);
            branch->shape()->view()->setHandle("branchShadowEnabled"); // Different handle for the only surface with shadow
            branch->setIsPhysicsObject(false);
            addChildObject(MCObjectPtr(branch), MCVector3dF(0, 0, branchHeight) * (i + 1));
        }
        else
        {
            branch->shape()->view()->setHasShadow(false);
            branch->setIsPhysicsObject(false);
            const auto offset = MCRandom::randomVector2d() * 5;
            addChildObject(MCObjectPtr(branch), MCVector3dF(0, 0, branchHeight) * (i + 1) + MCVector3dF(offset), MCRandom::getValue() * 360);
        }

        const float scale = r0 - (r0 - r1) / branches * i;
        branch->shape()->view()->setScale(MCVector3dF(scale, scale, 1.0f));
    }
}
