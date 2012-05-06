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

#include "ailogic.hpp"
#include "car.hpp"
#include "checkeredflag.hpp"
#include "inputhandler.hpp"
#include "layers.hpp"
#include "offtrackdetector.hpp"
#include "race.hpp"
#include "speedometer.hpp"
#include "startlights.hpp"
#include "startlightsoverlay.hpp"
#include "timingoverlay.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackobject.hpp"
#include "tracktile.hpp"

#include "../common/config.hpp"

#include <MiniCore/Core/MCCamera>
#include <MiniCore/Core/MCFrictionGenerator>
#include <MiniCore/Core/MCObject>
#include <MiniCore/Core/MCSurface>
#include <MiniCore/Core/MCSpringForceGenerator>
#include <MiniCore/Core/MCTextureManager>
#include <MiniCore/Core/MCTypes>
#include <MiniCore/Core/MCWorld>

#include <algorithm>
#include <cassert>

Scene::Scene(MCUint numCars)
  : m_race()
  , m_pActiveTrack(nullptr)
  , m_pWorld(new MCWorld)
  , m_pTimingOverlay(nullptr)
  , m_pSpeedometer(nullptr)
  , m_pStartlights(new Startlights(m_race))
  , m_pStartlightsOverlay(new StartlightsOverlay(*m_pStartlights))
  , m_pCheckeredFlag(new CheckeredFlag)
  , m_cameraBaseOffset(0)
{
    // Create and add cars.
    assert(numCars);
    for (MCUint i = 0; i < numCars; i++)
    {
        Car * car = nullptr;
        if (i == 0)
        {
            car = new Car(MCTextureManager::instance().surface("car001"), i);
            car->setPower(7000.0f);
        }
        else
        {
            car = new Car(MCTextureManager::instance().surface("car002"), i);
            m_aiLogic.push_back(new AiLogic(*car));
            car->setPower(7000.0f + i * 100.0f);
        }

        car->setLayer(Layers::Cars);

        m_cars.push_back(car);
        m_race.addCar(*car);

        m_offTrackDetectors.push_back(new OffTrackDetector(*car));
    }

    m_pStartlightsOverlay->setDimensions(
        Config::Game::WINDOW_WIDTH, Config::Game::WINDOW_HEIGHT);

    m_pCheckeredFlag->setDimensions(
        Config::Game::WINDOW_WIDTH, Config::Game::WINDOW_HEIGHT);
}

void Scene::updateFrame(InputHandler & handler,
    MCCamera & camera, float timeStep)
{
    if (m_race.started())
    {
        processUserInput(handler);

        updateAiLogic();
    }

    updateWorld(timeStep);

    updateRace();

    for (OffTrackDetector * otd : m_offTrackDetectors)
    {
        otd->update();
    }

    updateCameraLocation(camera);
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
    const int offsetAmplification = 10;
    const int smooth = 5;
    m_cameraBaseOffset +=
        (m_cars.at(0)->velocity().lengthFast() - m_cameraBaseOffset) / smooth;
    p += m_cars.at(0)->direction() * m_cameraBaseOffset * offsetAmplification;
    camera.setPos(p.i(), p.j());
}

void Scene::processUserInput(InputHandler & handler)
{
    bool actionTaken = false;
    bool steering    = false;

    m_cars.at(0)->clearStatuses();

    // Handle turning
    if (handler.getActionState(0, InputHandler::IA_LEFT))
    {
        m_cars.at(0)->turnLeft();
        actionTaken = true;
        steering    = true;
    }
    else if (handler.getActionState(0, InputHandler::IA_RIGHT))
    {
        m_cars.at(0)->turnRight();
        actionTaken = true;
        steering    = true;
    }

    // Handle accelerating / braking
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

    if (!steering)
    {
        m_cars.at(0)->noSteering();
    }
}

void Scene::updateAiLogic()
{
    for (AiLogic * ai : m_aiLogic)
    {
        const bool isRaceCompleted = m_race.timing().raceCompleted(ai->car().index());
        ai->update(isRaceCompleted);
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

    for (AiLogic * ai : m_aiLogic)
    {
        ai->setTrack(activeTrack);
    }

    for (OffTrackDetector * otd : m_offTrackDetectors)
    {
        otd->setTrack(activeTrack);
    }
}

void Scene::setWorldDimensions()
{
    assert(m_pWorld);
    assert(m_pActiveTrack);

    // Update world dimensions according to the
    // active track.
    const MCUint minX = 0;
    const MCUint maxX = m_pActiveTrack->width();
    const MCUint minY = 0;
    const MCUint maxY = m_pActiveTrack->height();
    const MCUint minZ = 0;
    const MCUint maxZ = 1000;

    const MCFloat metersPerPixel = 0.05f;
    m_pWorld->setDimensions(minX, maxX, minY, maxY, minZ, maxZ, metersPerPixel);
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
//        std::vector<Car *> randomized = m_cars;
//        std::random_shuffle(randomized.begin(), randomized.end());

        // Reverse order
        std::vector<Car *> order = m_cars;
        std::reverse(order.begin(), order.end());

        // Position the cars into two queues.
        for (MCUint i = 0; i < order.size(); i++)
        {
            MCFloat rowPos = (i / 2) * tileWidth;
            MCFloat colPos = (i % 2) * tileHeight / 3 - tileHeight / 6;

            order.at(i)->translate(
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
    m_race.setLapCount(2);
    m_race.init();
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
    m_pTimingOverlay->setRace(m_race);
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

    if (m_race.checkeredFlagEnabled())
    {
        assert(m_pCheckeredFlag);
        m_pCheckeredFlag->render();
    }
}

Scene::~Scene()
{
    for (Car * car : m_cars)
    {
        delete car;
    }

    for (AiLogic * ai : m_aiLogic)
    {
        delete ai;
    }

    for (OffTrackDetector * otd : m_offTrackDetectors)
    {
        delete otd;
    }

    delete m_pStartlights;
    delete m_pStartlightsOverlay;
}
