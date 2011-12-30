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

#include "scene.h"

#include "car.h"
#include "inputhandler.h"
#include "layers.h"
#include "race.h"
#include "track.h"
#include "trackdata.h"
#include "trackobject.h"
#include "tracktile.h"
#include <MiniCore/Core/MCCamera>
#include <MiniCore/Core/MCFrictionGenerator>
#include <MiniCore/Core/MCObject>
#include <MiniCore/Core/MCSurface>
#include <MiniCore/Core/MCSpringForceGenerator>
#include <MiniCore/Core/MCTypes>
#include <MiniCore/Core/MCWorld>

#include <cassert>

Scene::Scene(MCSurface & carSurface)
: m_pActiveTrack(nullptr)
, m_pWorld(new MCWorld)
, m_car(&carSurface)
, m_testCar(&carSurface)
, m_cameraBaseOffset(0)
{
    m_car.setLayer(Layers::Cars);
    m_race.addCar(&m_car);
}

void Scene::updateFrame(InputHandler & handler,
    MCCamera & camera, float timeStep)
{
    // Process input
    bool actionTaken = false;
    if (handler.getActionState(0, InputHandler::IA_LEFT))
    {
        m_car.turnLeft();
        actionTaken = true;
    }
    else if (handler.getActionState(0, InputHandler::IA_RIGHT))
    {
        m_car.turnRight();
        actionTaken = true;
    }

    if (handler.getActionState(0, InputHandler::IA_UP))
    {
        m_car.accelerate();
        actionTaken = true;
    }
    else if (handler.getActionState(0, InputHandler::IA_DOWN))
    {
        m_car.brake();
        actionTaken = true;
    }

    if (!actionTaken)
    {
        m_car.noAction();
    }

    // Step time
    m_pWorld->stepTime(timeStep);

    // Update camera location with respect to the car speed.
    // Make changes a bit smoother so that an abrupt decrease
    // in the speed won't look bad.
    MCVector2d<MCFloat> p(m_car.location());
    const int offsetAmplification = 20;
    const int smooth = 5;
    m_cameraBaseOffset +=
        (m_car.velocity().lengthFast() - m_cameraBaseOffset) / smooth;
    p += m_car.direction() * m_cameraBaseOffset * offsetAmplification;
    camera.setPos(p.i(), p.j());

    // Update race situation
    m_race.update();
}

void Scene::setActiveTrack(Track & activeTrack)
{
    m_pActiveTrack = &activeTrack;

    // TODO: Remove objects
    // TODO: Removing not inserted objects results in a
    // crash because ObjectTree can't handle it.
    //m_car.removeFromWorldNow();

    // Update world dimensions according to the
    // active track.
    const MCUint MIN_X = 0;
    const MCUint MAX_X = m_pActiveTrack->width();
    const MCUint MIN_Y = 0;
    const MCUint MAX_Y = m_pActiveTrack->height();
    const MCUint MIN_Z = 0;
    const MCUint MAX_Z = 1000;

    m_pWorld->setDimensions(MIN_X, MAX_X, MIN_Y, MAX_Y, MIN_Z, MAX_Z);
    m_race.setTrack(&activeTrack);
    m_race.init();

    // Add objects to the world

    m_car.addToWorld();
    m_testCar.addToWorld();

    if (m_pActiveTrack->trackData().route().length() > 0)
    {
        const MCFloat x =
            m_pActiveTrack->trackData().route().get(0)->location().x();
        const MCFloat y =
            m_pActiveTrack->trackData().route().get(0)->location().y();

        m_car.translate(MCVector2d<MCFloat>(x, y));
        m_testCar.translate(MCVector2d<MCFloat>(x, y + 50));
    }

    const unsigned int trackObjectCount =
        m_pActiveTrack->trackData().objects().count();
    for (unsigned int i = 0; i < trackObjectCount; i++)
    {
        TrackObject & trackObject = static_cast<TrackObject &>(
            m_pActiveTrack->trackData().objects().object(i));
        MCObject & mcObject = trackObject.object();
        mcObject.setLayer(Layers::Cars);
        mcObject.addToWorld();
        mcObject.translate(mcObject.initialLocation());
    }
}

Track & Scene::activeTrack() const
{
    assert(m_pActiveTrack);
    return *m_pActiveTrack;
}

MCWorld & Scene::world() const
{
    assert(m_pWorld);
    return *m_pWorld;
}

Scene::~Scene()
{
}
