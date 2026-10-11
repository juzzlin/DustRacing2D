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

#include <MCCircleShape>
#include <MCGLColor>
#include <MCMathUtil>
#include <MCPhysicsComponent>
#include <MCRandom>
#include <MCShape>
#include <MCShapeView>
#include <MCSurface>

#include <algorithm>
#include <cmath>

namespace {

const float treeBodyRadius = 8;

constexpr float twoPi = 6.2831853f;

/*! \return tint for a layer of the canopy.
 *  \param height Relative height of the layer: 0.0 is the lowest and 1.0 is the topmost layer.
 *  \param hue Per-tree variation of the green. */
MCGLColor layerTint(float height, float hue)
{
    // Lower layers are in the shade of the canopy and get bluish sky light,
    // upper layers catch the warm sunlight.
    const float light = 0.5f + 0.8f * std::pow(height, 0.6f);
    const float sun = std::clamp((height - 0.6f) / 0.4f, 0.0f, 1.0f);
    const float shade = 1.0f - height;
    return {
        light * hue * (1.0f + 0.10f * sun),
        light * (1.0f + 0.05f * sun),
        light * (1.0f - 0.20f * sun + 0.15f * shade)
    };
}

// Wind blows the same way over the whole track and its gusts travel along it.
const MCVector2dF windDirection = MCVector2dF(1.0f, 0.4f).normalized();
const float gustFrequency = 1.3f; // rad/s
const float gustWaveNumber = 0.004f; // rad/world unit
const float leanAmplitude = 1.0f; // Constant lean of the topmost layer
const float gustAmplitude = 2.5f; // Sway of the topmost layer in the wind direction
const float crossAmplitude = 1.0f; // Sway of the topmost layer across the wind direction
const float flutterAmplitude = 4.0f; // Rotation of the topmost layer in degrees

} // namespace

Tree::Tree(MCSurfacePtr surface, float r0, float r1, float treeHeight, size_t branches)
  : MCObject(std::make_shared<MCCircleShape>(nullptr, treeBodyRadius), "tree")
  , m_phase(MCRandom::getValue() * twoPi)
{
    physicsComponent().setMass(1, true); // Stationary
    physicsComponent().setRestitution(0.25f);

    setIsPhysicsObject(true);

    shape()->setRadius(treeBodyRadius);

    const float branchHeight = treeHeight / branches;
    const float hue = 0.85f + 0.3f * MCRandom::getValue();
    for (size_t i = 0; i < branches; i++)
    {
        const auto branch = std::make_shared<MCObject>(surface, i == 0 ? "treeRoot" : "treeBranch");
        const float height = branches > 1 ? static_cast<float>(i) / (branches - 1) : 1.0f;
        branch->shape()->view()->setColor(layerTint(height, hue));

        if (i == 0)
        {
            branch->shape()->view()->setHasShadow(true);
            branch->shape()->view()->setHandle("branchShadowEnabled"); // Different handle for the only surface with shadow
            branch->setIsPhysicsObject(false);
            const auto location = MCVector3dF(0, 0, branchHeight) * (i + 1);
            addChildObject(branch, location);
            m_layers.push_back({ branch, location, 0, height });
        }
        else
        {
            branch->shape()->view()->setHasShadow(false);
            branch->setIsPhysicsObject(false);
            const auto offset = MCRandom::randomVector2d() * 5;
            const auto location = MCVector3dF(0, 0, branchHeight) * (i + 1) + MCVector3dF(offset);
            const float angle = MCRandom::getValue() * 360;
            addChildObject(branch, location, angle);
            m_layers.push_back({ branch, location, angle, height });
        }

        const float scale = r0 - (r0 - r1) / branches * i;
        branch->shape()->view()->setScale({ scale, scale, 1.0f });
    }
}

void Tree::onStepTime(int step)
{
    m_time += static_cast<float>(step) / 1000;

    const float distanceAlongWind = location().i() * windDirection.i() + location().j() * windDirection.j();
    const float gustPhase = gustFrequency * m_time - gustWaveNumber * distanceAlongWind + m_phase;
    const float gust = leanAmplitude + gustAmplitude * (0.5f + 0.5f * std::sin(gustPhase));
    const float cross = crossAmplitude * std::sin(2.3f * gustPhase);
    const MCVector2dF crossDirection(-windDirection.j(), windDirection.i());

    // The tree is upright in its own coordinates, so wind needs to be rotated to them.
    const auto sway = MCMathUtil::rotatedVector(windDirection * gust + crossDirection * cross, -angle());

    for (size_t i = 0; i < m_layers.size(); i++)
    {
        auto && layer = m_layers[i];
        // Higher layers bend more.
        const float bend = layer.height * layer.height;
        layer.object->translateRelative(layer.restLocation + MCVector3dF(sway * bend));
        layer.object->rotateRelative(layer.restAngle + flutterAmplitude * layer.height * std::sin(3.1f * m_time + 0.7f * i + m_phase));
    }

    // Apply the new relative transforms to the layers.
    rotate(angle());
}
