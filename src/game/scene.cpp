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

#include "scene.hpp"

#include "car.hpp"
#include "inputhandler.hpp"
#include "layers.hpp"
#include "race.hpp"
#include "speedometer.hpp"
#include "startlights.hpp"
#include "startlightsoverlay.hpp"
#include "timingoverlay.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackobject.hpp"
#include "tracktile.hpp"

#include <MiniCore/Core/MCCamera>
#include <MiniCore/Core/MCFrictionGenerator>
#include <MiniCore/Core/MCObject>
#include <MiniCore/Core/MCSurface>
#include <MiniCore/Core/MCSpringForceGenerator>
#include <MiniCore/Core/MCTypes>
#include <MiniCore/Core/MCWorld>

#include <algorithm>
#include <cassert>

Scene::Scene(MCSurface & carSurface, MCUint numCars)
  : m_pActiveTrack(nullptr)
  , m_pWorld(new MCWorld)
  , m_pTimingOverlay(nullptr)
  , m_pSpeedometer(nullptr)
  , m_pStartlights(nullptr)
  , m_pStartlightsOverlay(nullptr)
  , m_cameraBaseOffset(0)
{
    // Create and add cars.
    assert(numCars);
    for (MCUint i = 0; i < numCars; i++)
    {
        Car * car = new Car(carSurface, i);
        car->setLayer(Layers::Cars);
        m_cars.push_back(car);
        m_race.addCar(*car);
    }
}

void Scene::updateFrame(InputHandler & handler,
    MCCamera & camera, float timeStep)
{
    processUserInput(handler);

    updateCameraLocation(camera);

    updateWorld(timeStep);

    updateRace();
}

void Scene::updateWorld(float timeStep)
{
    // Step time
    m_pWorld->stepTime(timeStep);
}

void Scene::updateRace()
{    
    m_pStartlights->update();

    // Update race situation
    m_race.update();
}

void Scene::updateCameraLocation(MCCamera & camera)
{
    // Update camera location with respect to the car speed.
    // Make changes a bit smoother so that an abrupt decrease
    // in the speed won't look bad.
    MCVector2d<MCFloat> p(m_cars.at(0)->location());
    const int offsetAmplification = 20;
    const int smooth = 5;
    m_cameraBaseOffset +=
        (m_cars.at(0)->velocity().lengthFast() - m_cameraBaseOffset) / smooth;
    p += m_cars.at(0)->direction() * m_cameraBaseOffset * offsetAmplification;
    camera.setPos(p.i(), p.j());
}

void Scene::processUserInput(InputHandler & handler)
{
    bool actionTaken = false;
    if (handler.getActionState(0, InputHandler::IA_LEFT))
    {
        m_cars.at(0)->turnLeft();
        actionTaken = true;
    }
    else if (handler.getActionState(0, InputHandler::IA_RIGHT))
    {
        m_cars.at(0)->turnRight();
        actionTaken = true;
    }

    if (handler.getActionState(0, InputHandler::IA_UP))
    {
        m_cars.at(0)->accelerate();
        actionTaken = true;
    }
    else if (handler.getActionState(0, InputHandler::IA_DOWN))
    {
        m_cars.at(0)->brake();
        actionTaken = true;
    }

    if (!actionTaken)
    {
        m_cars.at(0)->noAction();
    }
}

void Scene::setActiveTrack(Track & activeTrack)
{
    m_pActiveTrack = &activeTrack;

    // TODO: Remove objects
    // TODO: Removing not inserted objects results in a
    // crash because ObjectTree can't handle it.
    //m_car.removeFromWorldNow();

    setWorldDimensions();

    addCarsToWorld();

    translateCarsToStartPositions();

    addTrackObjectsToWorld();

    initRace();
}

void Scene::setWorldDimensions()
{
    assert(m_pWorld);
    assert(m_pActiveTrack);

    // Update world dimensions according to the
    // active track.
    const MCUint MIN_X = 0;
    const MCUint MAX_X = m_pActiveTrack->width();
    const MCUint MIN_Y = 0;
    const MCUint MAX_Y = m_pActiveTrack->height();
    const MCUint MIN_Z = 0;
    const MCUint MAX_Z = 1000;

    m_pWorld->setDimensions(MIN_X, MAX_X, MIN_Y, MAX_Y, MIN_Z, MAX_Z);
}

void Scene::addCarsToWorld()
{
    // Add objects to the world
    for (Car * car : m_cars)
    {
        car->addToWorld();
    }
}

void Scene::translateCarsToStartPositions()
{
    // TODO: Currently works only horizontally and to the right..

    assert(m_pActiveTrack);

    if (m_pActiveTrack->trackData().route().length() > 0)
    {
        const MCFloat startTileX =
            m_pActiveTrack->trackData().route().get(0)->location().x();
        const MCFloat startTileY =
            m_pActiveTrack->trackData().route().get(0)->location().y();
        const MCFloat tileWidth  = TrackTile::TILE_W;
        const MCFloat tileHeight = TrackTile::TILE_H;

        // Randomize the order
        std::vector<Car *> randomized = m_cars;
        std::random_shuffle(randomized.begin(), randomized.end());

        // Position the cars into two queues.
        for (MCUint i = 0; i < randomized.size(); i++)
        {
            MCFloat rowPos = (i / 2) * tileWidth;
            MCFloat colPos = (i % 2) * tileHeight / 3 - tileHeight / 6;

            randomized.at(i)->translate(
                MCVector2d<MCFloat>(
                    startTileX - rowPos,
                    startTileY + colPos));
        }
    }
}

void Scene::addTrackObjectsToWorld()
{
    assert(m_pActiveTrack);

    const unsigned int trackObjectCount =
        m_pActiveTrack->trackData().objects().count();

    for (unsigned int i = 0; i < trackObjectCount; i++)
    {
        TrackObject & trackObject = static_cast<TrackObject &>(
            m_pActiveTrack->trackData().objects().object(i));
        MCObject & mcObject = trackObject.object();
        mcObject.addToWorld();
        mcObject.translate(mcObject.initialLocation());
    }
}

void Scene::initRace()
{
    assert(m_pActiveTrack);

    m_race.setTrack(*m_pActiveTrack);
    m_race.init();
    m_race.start();
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

void Scene::setTimingOverlay(TimingOverlay & timingOverlay)
{
    m_pTimingOverlay = &timingOverlay;
    m_pTimingOverlay->setTiming(m_race.timing());
    m_pTimingOverlay->setCarToFollow(*m_cars.at(0));
}

void Scene::setSpeedometer(Speedometer & speedometer)
{
    m_pSpeedometer = &speedometer;
    m_pSpeedometer->setCarToFollow(*m_cars.at(0));
}

void Scene::setStartlights(Startlights & startlights)
{
    m_pStartlights = &startlights;
}

void Scene::setStartlightsOverlay(StartlightsOverlay & startlightsOverlay)
{
    m_pStartlightsOverlay = &startlightsOverlay;
}

TimingOverlay & Scene::timingOverlay() const
{
    assert(m_pTimingOverlay);
    return *m_pTimingOverlay;
}

void Scene::render(MCCamera & camera)
{
    assert(m_pActiveTrack);
    m_pActiveTrack->render(&camera);

    assert(m_pWorld);
    m_pWorld->renderShadows(&camera);
    m_pWorld->render(&camera);

    assert(m_pTimingOverlay);
    m_pTimingOverlay->render();

    assert(m_pSpeedometer);
    m_pSpeedometer->render();

    assert(m_pStartlightsOverlay);
    m_pStartlightsOverlay->render();
}

Scene::~Scene()
{
    for (Car * car : m_cars)
    {
        delete car;
    }
}
