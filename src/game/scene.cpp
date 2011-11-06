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

#include "car.h"
#include "layers.h"
#include "scene.h"
#include "track.h"
#include <MiniCore/Core/MCSurface>
#include <MiniCore/Core/MCTypes>
#include <MiniCore/Core/MCWorld>

Scene::Scene(MCSurface * pCarSurface)
: m_pActiveTrack(nullptr)
, m_pCar(new Car(pCarSurface))
, m_pWorld(new MCWorld())
{
    m_pCar->setLayer(Layers::Cars);
}

void Scene::updateFrame(float timeStep)
{
    m_pWorld->stepTime(timeStep);
}

void Scene::setActiveTrack(Track * activeTrack)
{
    m_pActiveTrack = activeTrack;

    // TODO: Remove objects
    // TODO: Removing not inserted objects results in a
    // crash because Quadtree can't handle it.
    //m_pCar->removeFromWorldNow();

    // Update world dimensions according to the
    // active track.
    const MCUint MIN_X = 0;
    const MCUint MAX_X = m_pActiveTrack->width();
    const MCUint MIN_Y = 0;
    const MCUint MAX_Y = m_pActiveTrack->height();
    const MCUint MIN_Z = 0;
    const MCUint MAX_Z = 1000;

    m_pWorld->setDimensions(MIN_X, MAX_X, MIN_Y, MAX_Y, MIN_Z, MAX_Z);
    m_pCar->addToWorld();
}

Track * Scene::activeTrack() const
{
    return m_pActiveTrack;
}

MCWorld * Scene::world() const
{
    return m_pWorld;
}

Scene::~Scene()
{
    delete m_pCar;
    delete m_pWorld;
}
