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

#include <MCAssetManager>
#include <MCCircleShape>
#include <MCGLColor>
#include <MCMathUtil>
#include <MCPhysicsComponent>
#include <MCRandom>
#include <MCShape>
#include <MCShapeView>
#include <MCSurface>
#include <MCSurfaceManager>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace {

const float treeBodyRadius = 8;

const float layerSpacing = 10;

const float trunkScale = 0.5f;

const MCGLColor trunkTint { 0.7f, 0.7f, 0.7f };

//! Shadows fall away from the light. The offset is relative to the height of the crown.
const MCVector2dF shadowDirection = MCVector2dF(1.0f, -1.0f) * 0.08f;

constexpr float twoPi = 6.2831853f;

struct SpeciesTraits
{
    //! Surfaces of the layers of the crown. Each layer gets a random one.
    std::vector<std::string> surfaces;

    //! Range of tree heights in world units.
    float minHeight, maxHeight;

    //! Range of the scale of the widest layer of the crown.
    float minRadius, maxRadius;

    //! Maximum random horizontal offset of the layers.
    float layerOffset;

    //! Relative height where the crown starts. The trunk is visible below it.
    float crownBase;

    //! Width of the crown relative to its widest layer.
    //! \param height Relative height within the crown: 0.0 is its bottom and 1.0 its top.
    float (*profile)(float height);
};

const SpeciesTraits & speciesTraits(Tree::Species species)
{
    static const SpeciesTraits deciduous {
        { "tree", "tree2", "tree3" },
        160, 320,
        1.1f, 1.6f,
        7.0f,
        0.35f,
        // An ellipsoid
        [](float height) { return 0.3f + 0.7f * std::sqrt(std::max(0.0f, 1.0f - (2 * height - 1) * (2 * height - 1))); }
    };

    static const SpeciesTraits pine {
        { "pine", "pine2" },
        240, 400,
        1.1f, 1.5f,
        1.5f,
        0.1f,
        // A narrow cone
        [](float height) { return 0.08f + 0.92f * std::pow(1.0f - height, 1.2f); }
    };

    return species == Tree::Species::Pine ? pine : deciduous;
}

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

Tree::Tree(Species species)
  : MCObject(std::make_shared<MCCircleShape>(nullptr, treeBodyRadius), "tree")
  , m_phase(MCRandom::getValue() * twoPi)
{
    physicsComponent().setMass(1, true); // Stationary
    physicsComponent().setRestitution(0.25f);

    setIsPhysicsObject(true);

    shape()->setRadius(treeBodyRadius);

    const auto & traits = speciesTraits(species);
    const float treeHeight = traits.minHeight + (traits.maxHeight - traits.minHeight) * MCRandom::getValue();
    const float radius = traits.minRadius + (traits.maxRadius - traits.minRadius) * MCRandom::getValue();
    const float hue = 0.85f + 0.3f * MCRandom::getValue();
    const float crownBase = treeHeight * traits.crownBase;

    const auto addLayer = [this, treeHeight](MCObjectPtr layer, MCVector3dF location, float angle) {
        layer->setIsPhysicsObject(false);
        layer->shape()->view()->setHasShadow(false);
        addChildObject(layer, location, angle);
        m_layers.push_back({ layer, location, angle, location.k() / treeHeight });
    };

    for (float z = layerSpacing; z < crownBase; z += layerSpacing)
    {
        const auto layer = std::make_shared<MCObject>(MCAssetManager::surfaceManager().surface("trunk"), "treeTrunk");
        layer->shape()->view()->setColor(trunkTint);
        layer->shape()->view()->setScale({ trunkScale, trunkScale, 1.0f });
        addLayer(layer, { 0, 0, z }, MCRandom::getValue() * 360);
    }

    const auto crownLayers = std::max(static_cast<size_t>((treeHeight - crownBase) / layerSpacing), size_t { 2 });
    MCObjectPtr widestLayer;
    float widestScale = 0;
    for (size_t i = 0; i < crownLayers; i++)
    {
        const auto & surfaceName = traits.surfaces.at(static_cast<size_t>(MCRandom::getValue() * traits.surfaces.size()) % traits.surfaces.size());
        const auto layer = std::make_shared<MCObject>(MCAssetManager::surfaceManager().surface(surfaceName), "treeBranch");
        const float crownHeight = static_cast<float>(i) / (crownLayers - 1);
        layer->shape()->view()->setColor(layerTint(crownHeight, hue));

        // Jitter the sizes so that the edges of the layers don't line up as rings.
        const float scale = radius * traits.profile(crownHeight) * (0.88f + 0.24f * MCRandom::getValue());
        layer->shape()->view()->setScale({ scale, scale, 1.0f });
        if (scale > widestScale)
        {
            widestScale = scale;
            widestLayer = layer;
        }

        const auto offset = MCVector3dF(MCRandom::randomVector2d() * traits.layerOffset);
        addLayer(layer, offset + MCVector3dF(0, 0, crownBase + layerSpacing * (i + 1)), MCRandom::getValue() * 360);
    }

    // The widest layer of the crown casts the shadow.
    widestLayer->shape()->view()->setHasShadow(true);
    widestLayer->shape()->view()->setHandle("branchShadowEnabled"); // Different handle for the only surface with shadow
    widestLayer->shape()->setShadowOffset(MCVector3dF(shadowDirection * (crownBase + (treeHeight - crownBase) / 2)));
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
