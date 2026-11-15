// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "grandstand.hpp"

#include <MCAssetManager>
#include <MCPhysicsComponent>
#include <MCRandom>
#include <MCRectShape>
#include <MCShapeView>
#include <MCSurface>
#include <MCSurfaceManager>
#include <MCSurfaceView>
#include <mcgltexcoord.hh>

#include <array>
#include <cmath>
#include <string>

namespace {

//! Size in world units. Must match the layout in scripts/generate-textures.
const float grandstandSize { 128 };

//! The stand slopes up from the front at the track to the back.
const float frontZ { 5 };
const float backZ { 25 };

//! Each section shows two seats of the 16 seats of a row, so that the edges of the sections don't cut spectators.
constexpr size_t sectionCount { 8 };

enum Frame : size_t
{
    Calm,
    CalmAlternate,
    Excited,
    ExcitedAlternate,
    Cheering,
    FrameCount
};

//! Cars closer than this make the crowd excited.
const float excitementRange { 350 };

//! How fast the excitement fades away per second.
const float excitementDecay { 0.35f };

//! Speed of the wave in sections per second.
const float waveSpeed { 7 };

const float minWaveInterval { 15 };
const float maxWaveInterval { 45 };

using SectionSurfaces = std::array<std::array<MCSurfacePtr, sectionCount>, FrameCount>;

//! Surfaces of each frame and section, shared by all grandstands.
const SectionSurfaces & sectionSurfaces()
{
    static SectionSurfaces surfaces;
    if (!surfaces[0][0])
    {
        const float sectionWidth { grandstandSize / sectionCount };
        for (size_t frame = 0; frame < FrameCount; frame++)
        {
            const auto frameSurface { MCAssetManager::surfaceManager().surface("grandstandCrowd" + std::to_string(frame)) };
            for (size_t section = 0; section < sectionCount; section++)
            {
                const float u0 { static_cast<float>(section) / sectionCount };
                const float u1 { static_cast<float>(section + 1) / sectionCount };
                const MCGLTexCoord texCoords[4] { { u0, 0 }, { u0, 1 }, { u1, 1 }, { u1, 0 } };
                // Unique handles, because the renderer batches the objects by them
                const std::string handle { "grandstandCrowd" + std::to_string(frame) + "_" + std::to_string(section) };
                const auto surface { std::make_shared<MCSurface>(handle, frameSurface->material(), sectionWidth, grandstandSize, texCoords, frontZ, backZ, backZ, frontZ) };
                surface->setShaderProgram(frameSurface->shaderProgram());
                surface->setShadowShaderProgram(frameSurface->shadowShaderProgram());
                surfaces[frame][section] = surface;
            }
        }
    }

    return surfaces;
}

float randomInterval(float min, float max)
{
    return min + (max - min) * MCRandom::getValue();
}

} // namespace

std::vector<MCVector2dF> Grandstand::m_carLocations;

Grandstand::Grandstand()
  : MCObject { std::make_shared<MCRectShape>(nullptr, grandstandSize, grandstandSize), "grandstand" }
  , m_timeToNextWave { randomInterval(minWaveInterval, maxWaveInterval) }
{
    physicsComponent().setMass(100000);
    physicsComponent().setXYFriction(1.0f);
    physicsComponent().setRestitution(0.5f);

    setIsPhysicsObject(true);

    const float sectionWidth { grandstandSize / sectionCount };
    for (size_t i = 0; i < sectionCount; i++)
    {
        const auto object { std::make_shared<MCObject>(sectionSurfaces()[Calm][i], "grandstandSection") };
        object->setIsPhysicsObject(false);
        object->setBypassCollisions(true);
        addChildObject(object, { -grandstandSize / 2 + sectionWidth * (static_cast<float>(i) + 0.5f), 0, 0 });

        Section section;
        section.object = object;
        section.nextPoseChange = randomInterval(0.5f, 3.0f);
        m_sections.push_back(section);
    }
}

void Grandstand::setCarLocations(std::vector<MCVector2dF> locations)
{
    m_carLocations = std::move(locations);
}

void Grandstand::setFrame(Section & section, size_t frame)
{
    if (section.frame != frame)
    {
        section.frame = frame;
        const auto view { std::static_pointer_cast<MCSurfaceView>(section.object->shape()->view()) };
        const size_t index { static_cast<size_t>(&section - m_sections.data()) };
        view->setSurface(sectionSurfaces()[frame][index]);
    }
}

void Grandstand::updateExcitement(Section & section, float timeStep)
{
    section.excitement = std::max(0.0f, section.excitement - excitementDecay * timeStep);

    const MCVector2dF location { section.object->location() };
    for (auto && carLocation : m_carLocations)
    {
        const float distance { (carLocation - location).lengthFast() };
        if (distance < excitementRange)
        {
            section.excitement = std::max(section.excitement, 1.0f - distance / excitementRange);
        }
    }
}

void Grandstand::onStepTime(int step)
{
    const float timeStep { static_cast<float>(step) / 1000 };

    // Start a wave every now and then. It travels across the stand.
    if (m_wavePosition < 0)
    {
        m_timeToNextWave -= timeStep;
        if (m_timeToNextWave <= 0)
        {
            m_wavePosition = 0;
            m_timeToNextWave = randomInterval(minWaveInterval, maxWaveInterval);
        }
    }
    else
    {
        m_wavePosition += waveSpeed * timeStep;
        if (m_wavePosition > sectionCount + 1)
        {
            m_wavePosition = -1;
        }
    }

    for (size_t i = 0; i < m_sections.size(); i++)
    {
        auto && section { m_sections.at(i) };
        updateExcitement(section, timeStep);

        if (m_wavePosition >= 0 && std::fabs(static_cast<float>(i) + 0.5f - m_wavePosition) < 1.0f)
        {
            setFrame(section, Cheering);
            continue;
        }

        // The more excited the crowd, the faster people move
        section.nextPoseChange -= timeStep;
        if (section.nextPoseChange <= 0)
        {
            section.alternate = !section.alternate;
            if (section.excitement > 0.6f)
            {
                section.nextPoseChange = randomInterval(0.2f, 0.35f);
            }
            else if (section.excitement > 0.15f)
            {
                section.nextPoseChange = randomInterval(0.3f, 0.6f);
            }
            else
            {
                section.nextPoseChange = randomInterval(1.0f, 3.0f);
            }
        }

        if (section.excitement > 0.6f)
        {
            setFrame(section, section.alternate ? Cheering : Excited);
        }
        else if (section.excitement > 0.15f)
        {
            setFrame(section, section.alternate ? Excited : ExcitedAlternate);
        }
        else
        {
            setFrame(section, section.alternate ? Calm : CalmAlternate);
        }
    }
}
