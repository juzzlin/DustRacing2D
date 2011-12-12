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
#include "tracktile.h"
#include "../../common/trackdata.h"
#include <MiniCore/Core/MCCamera>
#include <MiniCore/Core/MCFrictionGenerator>
#include <MiniCore/Core/MCObject>
#include <MiniCore/Core/MCSurface>
#include <MiniCore/Core/MCSpringForceGenerator>
#include <MiniCore/Core/MCTypes>
#include <MiniCore/Core/MCWorld>

Scene::Scene(MCSurface * pCarSurface)
: m_pActiveTrack(nullptr)
, m_pWorld(new MCWorld)
, m_pCar(new Car(pCarSurface))
, m_pRace(new Race)
{
    m_pCar->setLayer(Layers::Cars);
    m_pRace->addCar(m_pCar);
}

void Scene::updateFrame(InputHandler * pHandler,
    MCCamera * pCamera, float timeStep)
{
    // Process input
    bool actionTaken = false;
    if (pHandler->getActionState(0, InputHandler::IA_LEFT))
    {
        m_pCar->turnLeft();
        actionTaken = true;
    }
    else if (pHandler->getActionState(0, InputHandler::IA_RIGHT))
    {
        m_pCar->turnRight();
        actionTaken = true;
    }

    if (pHandler->getActionState(0, InputHandler::IA_UP))
    {
        m_pCar->accelerate();
        actionTaken = true;
    }
    else if (pHandler->getActionState(0, InputHandler::IA_DOWN))
    {
        m_pCar->brake();
        actionTaken = true;
    }

    if (!actionTaken)
    {
        m_pCar->noAction();
    }

    // Step time
    m_pWorld->stepTime(timeStep);

    // Update camera location
    MCVector2d<MCFloat> p(m_pCar->location());
    const int offsetFactor = 20;
    p += m_pCar->direction() * m_pCar->velocity().lengthFast() * offsetFactor;
    pCamera->setPos(p.i(), p.j());

    // Update race situation
    m_pRace->update();
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
    m_pRace->setTrack(activeTrack);
    m_pRace->init();

    if (m_pActiveTrack->trackData().route().length() > 0)
    {
        const MCFloat x =
            m_pActiveTrack->trackData().route().get(0)->location().x();
        const MCFloat y =
            m_pActiveTrack->trackData().route().get(0)->location().y();

        m_pCar->translate(MCVector2d<MCFloat>(x, y));
    }
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
    delete m_pRace;
    delete m_pWorld;
}
