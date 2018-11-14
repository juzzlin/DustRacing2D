// This file is part of Dust Racing 2D.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#include "scene.hpp"

#include "ai.hpp"
#include "audioworker.hpp"
#include "bridge.hpp"
#include "car.hpp"
#include "carfactory.hpp"
#include "carsoundeffectmanager.hpp"
#include "checkeredflag.hpp"
#include "fadeanimation.hpp"
#include "game.hpp"
#include "inputhandler.hpp"
#include "intro.hpp"
#include "layers.hpp"
#include "mainmenu.hpp"
#include "messageoverlay.hpp"
#include "particlefactory.hpp"
#include "pit.hpp"
#include "race.hpp"
#include "renderer.hpp"
#include "settings.hpp"
#include "startlights.hpp"
#include "startlightsoverlay.hpp"
#include "statemachine.hpp"
#include "timingoverlay.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackobject.hpp"
#include "tracktile.hpp"

#include "../common/config.hpp"
#include "../common/targetnodebase.hpp"

#include <MenuManager>

#include <MCAssetManager>
#include <MCCamera>
#include <MCFrictionGenerator>
#include <MCGLAmbientLight>
#include <MCGLDiffuseLight>
#include <MCGLScene>
#include <MCGLShaderProgram>
#include <MCLogger>
#include <MCObjectFactory>
#include <MCObject>
#include <MCPhysicsComponent>
#include <MCShape>
#include <MCSurface>
#include <MCSurfaceView>
#include <MCTextureFont>

#include <MCWorld>
#include <MCWorldRenderer>

#include <QObject>
#include <QApplication>

#include <algorithm>
#include <cassert>
#include <memory>

using std::dynamic_pointer_cast;

// Default visible scene size.
int Scene::m_width  = 1024;
int Scene::m_height = 768;

static const float METERS_PER_UNIT = 0.05f;

Scene::Scene(Game & game, StateMachine & stateMachine, Renderer & renderer, MCWorld & world)
: m_game(game)
, m_stateMachine(stateMachine)
, m_renderer(renderer)
, m_messageOverlay(new MessageOverlay)
, m_race(game, NUM_CARS)
, m_activeTrack(nullptr)
, m_world(world)
, m_startlights(new Startlights)
, m_startlightsOverlay(new StartlightsOverlay(*m_startlights))
, m_checkeredFlag(new CheckeredFlag)
, m_mainMenu(nullptr)
, m_menuManager(nullptr)
, m_intro(new Intro)
, m_particleFactory(new ParticleFactory)
, m_fadeAnimation(new FadeAnimation)
{
    connect(m_startlights, &Startlights::raceStarted, &m_race, &Race::start);
    connect(m_startlights, &Startlights::animationEnded, &m_stateMachine, &StateMachine::endStartlightAnimation);

    connect(&m_stateMachine, &StateMachine::startlightAnimationRequested, m_startlights, &Startlights::beginAnimation);
    connect(&m_stateMachine, &StateMachine::fadeInRequested, m_fadeAnimation, &FadeAnimation::beginFadeIn);
    connect(&m_stateMachine, &StateMachine::fadeOutRequested, m_fadeAnimation, &FadeAnimation::beginFadeOut);
    connect(&m_stateMachine, &StateMachine::fadeOutFlashRequested, m_fadeAnimation, &FadeAnimation::beginFadeOutFlash);
    connect(&m_stateMachine, &StateMachine::soundsStopped, &m_race, &Race::stopEngineSounds);

    connect(m_fadeAnimation, &FadeAnimation::fadeValueChanged, &m_renderer, &Renderer::setFadeValue);
    connect(m_fadeAnimation, &FadeAnimation::fadeInFinished, &m_stateMachine, &StateMachine::endFadeIn);
    connect(m_fadeAnimation, &FadeAnimation::fadeOutFinished, &m_stateMachine, &StateMachine::endFadeOut);

    connect(&m_race, &Race::finished, &m_stateMachine, &StateMachine::finishRace);
    connect(&m_race, &Race::messageRequested, m_messageOverlay, static_cast<void(MessageOverlay::*)(QString)>(&MessageOverlay::addMessage));

    connect(m_startlights, &Startlights::messageRequested, m_messageOverlay, static_cast<void(MessageOverlay::*)(QString)>(&MessageOverlay::addMessage));
    connect(this, &Scene::listenerLocationChanged, &m_game.audioWorker(), &AudioWorker::setListenerLocation);

    m_game.audioWorker().connectAudioSource(m_race);

    for (int i = 0; i < 2; i++)
    {
        m_cameraOffset[i] = 0.0f;
        m_timingOverlay[i].setRace(m_race);
    }

    m_checkeredFlag->setDimensions(width(), height());
    m_intro->setDimensions(width(), height());
    m_startlightsOverlay->setDimensions(width(), height());
    m_messageOverlay->setDimensions(width(), height());

    m_world.setMetersPerUnit(METERS_PER_UNIT);

    MCAssetManager::textureFontManager().font(m_game.fontName()).setShaderProgram(
        m_renderer.program("text"));
    MCAssetManager::textureFontManager().font(m_game.fontName()).setShadowShaderProgram(
        m_renderer.program("textShadow"));

    const MCGLAmbientLight ambientLight(1.0, 0.9, 0.95, 0.75);
    const MCGLDiffuseLight diffuseLight(MCVector3dF(1.0, -1.0, -1.0), 1.0, 0.9, 0.5, 0.75);
    const MCGLDiffuseLight specularLight(MCVector3dF(1.0, -1.0, -1.0), 1.0, 1.0, 0.8, 0.9);

    MCGLScene & glScene = MCWorld::instance().renderer().glScene();
    glScene.setAmbientLight(ambientLight);
    glScene.setDiffuseLight(diffuseLight);
    glScene.setSpecularLight(specularLight);

    m_renderer.setFadeValue(0.0);

    createMenus();
}

void Scene::setupAudio(Car & car, int index)
{
    std::stringstream engine;
    engine << "carEngine" << index;

    std::stringstream hit;
    hit << "carHit" << index;

    std::stringstream skid;
    skid << "skid" << index;

    CarSoundEffectManager::MultiSoundHandles handles;
    handles.engineSoundHandle = engine.str().c_str();
    handles.hitSoundHandle    = hit.str().c_str();
    handles.skidSoundHandle   = skid.str().c_str();

    CarSoundEffectManagerPtr sfx(new CarSoundEffectManager(car, handles));
    m_game.audioWorker().connectAudioSource(*sfx);
    car.setSoundEffectManager(sfx);
}

void Scene::createCars()
{
    m_race.removeCars();
    m_cars.clear();
    m_ai.clear();

    // Create and add cars.
    for (int i = 0; i < NUM_CARS; i++)
    {
        CarPtr car(CarFactory::buildCar(i, NUM_CARS, m_game));
        if (car)
        {
            if (!car->isHuman())
            {
                m_ai.push_back(AIPtr(new AI(*car)));
            }

            car->shape()->view()->setShaderProgram(m_renderer.program("car"));
            car->shape()->view()->object()->material()->setDiffuseCoeff(3.4f);

            setupAudio(*car, i);

            m_cars.push_back(car);
            m_race.addCar(*car);
        }
    }

    if (m_game.hasTwoHumanPlayers())
    {
        m_timingOverlay[1].setCarToFollow(*m_cars.at(1));
        m_crashOverlay[1].setCarToFollow(*m_cars.at(1));
    }

    m_timingOverlay[0].setCarToFollow(*m_cars.at(0));
    m_crashOverlay[0].setCarToFollow(*m_cars.at(0));
}

void Scene::setupMinimaps()
{
    const int minimapSize = m_width * 0.2f;

    const int minimapY = !m_game.hasTwoHumanPlayers() ? minimapSize : minimapSize / 2 + 10;

    for (int i = 0; i < 2; i++)
    {
        m_minimap[i].initialize(*m_cars[i], m_activeTrack->trackData().map(), minimapSize / 2 + 10, minimapY, minimapSize);
    }
}

int Scene::width()
{
    return Scene::m_width;
}

int Scene::height()
{
    return Scene::m_height;
}

void Scene::setSize(int width, int height)
{
    Scene::m_width  = width;
    Scene::m_height = height;
}

void Scene::createMenus()
{
    m_menuManager = new MTFH::MenuManager;

    m_mainMenu = MTFH::MenuPtr(new MainMenu(*m_menuManager, *this, width(), height()));
    connect(
        std::static_pointer_cast<MainMenu>(m_mainMenu).get(), &MainMenu::exitGameRequested, &m_game, &Game::exitGame);

    m_menuManager->addMenu(m_mainMenu);
    m_menuManager->enterMenu(m_mainMenu);
}

void Scene::updateFrame(InputHandler & handler, int step)
{
    if (m_stateMachine.state() == StateMachine::State::GameTransitionIn  ||
        m_stateMachine.state() == StateMachine::State::GameTransitionOut ||
        m_stateMachine.state() == StateMachine::State::DoStartlights     ||
        m_stateMachine.state() == StateMachine::State::Play)
    {
        if (m_activeTrack)
        {
            if (m_race.started())
            {
                processUserInput(handler);
                updateAi();
            }

            updateWorld(step);
            updateRace();

            if (m_game.hasTwoHumanPlayers())
            {
                for (int i = 0; i < 2; i++)
                {
                    updateCameraLocation(m_camera[i], m_cameraOffset[i], *m_cars.at(i));
                }
            }
            else
            {
                updateCameraLocation(m_camera[0], m_cameraOffset[0], *m_cars.at(0));
            }
        }
    }
    else if (m_stateMachine.state() == StateMachine::State::Menu)
    {
        m_menuManager->stepTime(step);
    }

    const float fadeValue = m_renderer.fadeValue();
    if (m_fadeAnimation->isFading())
    {
        MCGLScene & glScene = MCWorld::instance().renderer().glScene();
        glScene.setFadeValue(fadeValue);
    }
}

void Scene::updateOverlays()
{
    if (m_game.hasTwoHumanPlayers())
    {
        m_timingOverlay[1].update();
        m_crashOverlay[1].update();
    }

    m_timingOverlay[0].update();
    m_crashOverlay[0].update();

    m_messageOverlay->update();
}

void Scene::updateWorld(float timeStep)
{
    // Step time
    m_world.stepTime(timeStep);
}

void Scene::updateRace()
{
    // Update race situation
    m_race.update();

    emit listenerLocationChanged(m_cars[0]->location().i(), m_cars[0]->location().j());
}

void Scene::updateCameraLocation(MCCamera & camera, float & offset, MCObject & object)
{
    // Update camera location with respect to the car speed.
    // Make changes a bit smoother so that an abrupt decrease
    // in the speed won't look bad.
    MCVector2dF loc(object.location());

    const float offsetAmplification = m_game.hasTwoHumanPlayers() ? 9.6 : 13.8;
    const float smooth              = 0.2;

    offset += (object.physicsComponent().velocity().lengthFast() - offset) * smooth;
    loc    += object.direction() * offset * offsetAmplification;

    camera.setPos(loc.i(), loc.j());
}

void Scene::processUserInput(InputHandler & handler)
{
    for (int i = 0; i < (m_game.hasTwoHumanPlayers() ? 2 : 1); i++)
    {
        m_cars.at(i)->clearStatuses();

        // Handle accelerating / braking
        if (handler.getActionState(i, InputHandler::Action::Down))
        {
            if (!m_race.timing().raceCompleted(i))
            {
                m_cars.at(i)->brake();
            }
        }
        else if (handler.getActionState(i, InputHandler::Action::Up))
        {
            if (!m_race.timing().raceCompleted(i))
            {
                m_cars.at(i)->accelerate();
            }
        }

        // Handle turning
        if (handler.getActionState(i, InputHandler::Action::Left))
        {
            m_cars.at(i)->steer(Car::Steer::Left);
        }
        else if (handler.getActionState(i, InputHandler::Action::Right))
        {
            m_cars.at(i)->steer(Car::Steer::Right);
        }
        else
        {
            m_cars.at(i)->steer(Car::Steer::Neutral);
        }
    }
}

void Scene::updateAi()
{
    for (AIPtr ai : m_ai)
    {
        const bool isRaceCompleted = m_race.timing().raceCompleted(ai->car().index());
        ai->update(isRaceCompleted);
    }
}

void Scene::setupCameras(Track & activeTrack)
{
    m_world.renderer().removeParticleVisibilityCameras();
    if (m_game.hasTwoHumanPlayers())
    {
        for (int i = 0; i < 2; i++)
        {
            if (m_game.splitType() == Game::SplitType::Vertical)
            {
                m_camera[i].init(
                    Scene::width() / 2, Scene::height(), 0, 0, activeTrack.width(), activeTrack.height());
                m_world.renderer().addParticleVisibilityCamera(m_camera[i]);
            }
            else
            {
                m_camera[i].init(
                    Scene::width(), Scene::height() / 2, 0, 0, activeTrack.width(), activeTrack.height());
                m_world.renderer().addParticleVisibilityCamera(m_camera[i]);
            }
        }
    }
    else
    {
        m_camera[0].init(
            Scene::width(), Scene::height(), 0, 0, activeTrack.width(), activeTrack.height());
        m_world.renderer().addParticleVisibilityCamera(m_camera[0]);
    }
}

void Scene::setupAI(Track & activeTrack)
{
    for (AIPtr ai : m_ai)
    {
        ai->setTrack(activeTrack);
    }
}

void Scene::setActiveTrack(Track & activeTrack)
{
    m_activeTrack = &activeTrack;

    // Remove previous objects
    m_world.clear();

    setupCameras(activeTrack);

    setWorldDimensions();

    createCars();

    resizeOverlays();

    addCarsToWorld();

    addTrackObjectsToWorld();

    initRace();

    setupAI(activeTrack);

    setupMinimaps();
}

void Scene::setWorldDimensions()
{
    assert(m_activeTrack);

    // Update world dimensions according to the
    // active track.
    const unsigned int minX = 0;
    const unsigned int maxX = m_activeTrack->width();
    const unsigned int minY = 0;
    const unsigned int maxY = m_activeTrack->height();
    const unsigned int minZ = 0;
    const unsigned int maxZ = 1000;

    m_world.setDimensions(minX, maxX, minY, maxY, minZ, maxZ, METERS_PER_UNIT);
}

void Scene::addCarsToWorld()
{
    // Add objects to the world
    for (CarPtr car : m_cars)
    {
        car->addToWorld();
    }
}

void Scene::addTrackObjectsToWorld()
{
    createNormalObjects();

    createBridgeObjects();
}

void Scene::createNormalObjects()
{
    assert(m_activeTrack);

    for (unsigned int i = 0; i < m_activeTrack->trackData().objects().count(); i++)
    {
        auto trackObject = dynamic_pointer_cast<TrackObject>(m_activeTrack->trackData().objects().object(i));
        assert(trackObject);

        MCObject & object = trackObject->object();
        object.addToWorld();

        // Set the base Z of mesh objects at ground level instead of at the object center
        float baseZ = 0;
        if (object.shape() && object.shape()->view() && object.shape()->view()->object())
        {
            if (dynamic_cast<MCMesh*>(object.shape()->view()->object()))
            {
                baseZ = -object.shape()->view()->object()->minZ();
            }
        }

        object.translate(object.initialLocation() + MCVector3dF(0, 0, baseZ));
        object.rotate(object.initialAngle());

        if (auto pit = dynamic_cast<Pit *>(&object))
        {
            connect(pit, &Pit::pitStop, &m_race, &Race::pitStop);
        }
    }
}

void Scene::createBridgeObjects()
{
    assert(m_activeTrack);

    const MapBase & rMap = m_activeTrack->trackData().map();

    static const int w = TrackTile::TILE_W;
    static const int h = TrackTile::TILE_H;

    for (unsigned int j = 0; j <= rMap.rows(); j++)
    {
        for (unsigned int i = 0; i <= rMap.cols(); i++)
        {
            auto tile = dynamic_pointer_cast<TrackTile>(rMap.getTile(i, j));
            if (tile && tile->tileTypeEnum() == TrackTile::TT_BRIDGE)
            {
                MCObjectPtr bridge(new Bridge);

                bridge->translate(MCVector3dF(i * w + w / 2, j * h + h / 2, 0));
                bridge->rotate(tile->rotation());
                bridge->addToWorld();
                m_bridges.push_back(bridge);
            }
        }
    }
}

void Scene::resizeOverlays()
{
    if (m_game.hasTwoHumanPlayers())
    {
        if (m_game.splitType() == Game::SplitType::Vertical)
        {
            for (int i = 0; i < 2; i++)
            {
                m_timingOverlay[i].setDimensions(width() / 2, height());
                m_crashOverlay[i].setDimensions(width() / 2, height());
            }
        }
        else
        {
            for (int i = 0; i < 2; i++)
            {
                m_timingOverlay[i].setDimensions(width(), height() / 2);
                m_crashOverlay[i].setDimensions(width(), height() / 2);
            }
        }
    }
    else
    {
        m_timingOverlay[0].setDimensions(width(), height());
        m_crashOverlay[0].setDimensions(width(), height());
    }
}

void Scene::initRace()
{
    assert(m_activeTrack);
    m_race.init(*m_activeTrack, m_game.lapCount());
}

Track & Scene::activeTrack() const
{
    assert(m_activeTrack);
    return *m_activeTrack;
}

MTFH::MenuPtr Scene::trackSelectionMenu() const
{
    return m_menuManager->getMenuById("trackSelection");
}

void Scene::getSplitPositions(MCGLScene::SplitType & p0, MCGLScene::SplitType & p1)
{
    if (m_game.splitType() == Game::SplitType::Vertical)
    {
        p1 = MCGLScene::ShowOnLeft;
        p0 = MCGLScene::ShowOnRight;
    }
    else
    {
        p1 = MCGLScene::ShowOnTop;
        p0 = MCGLScene::ShowOnBottom;
    }
}

void Scene::renderTrack()
{
    switch (m_stateMachine.state())
    {
    case StateMachine::State::GameTransitionIn:
    case StateMachine::State::GameTransitionOut:
    case StateMachine::State::DoStartlights:
    case StateMachine::State::Play:
    {
        MCGLScene & glScene = MCWorld::instance().renderer().glScene();

        if (m_game.hasTwoHumanPlayers())
        {
            MCGLScene::SplitType p1, p0;
            getSplitPositions(p1, p0);

            glScene.setSplitType(p1);
            m_activeTrack->render(&m_camera[1]);

            glScene.setSplitType(p0);
            m_activeTrack->render(&m_camera[0]);

            glScene.setSplitType(MCGLScene::ShowFullScreen);
        }
        else
        {
            m_activeTrack->render(&m_camera[0]);
        }

        break;
    }
    default:
        break;
    };
}

void Scene::renderMenu()
{
    MCGLScene & glScene = MCWorld::instance().renderer().glScene();

    switch (m_stateMachine.state())
    {
    case StateMachine::State::DoIntro:

        glScene.setSplitType(MCGLScene::ShowFullScreen);

        m_intro->render();

        break;

    case StateMachine::State::Menu:
    case StateMachine::State::MenuTransitionOut:
    case StateMachine::State::MenuTransitionIn:

        glScene.setSplitType(MCGLScene::ShowFullScreen);

        m_menuManager->render();

        break;

    default:
        break;
    };
}

void Scene::renderCommonHUD()
{
    switch (m_stateMachine.state())
    {
    case StateMachine::State::GameTransitionIn:
    case StateMachine::State::GameTransitionOut:
    case StateMachine::State::DoStartlights:
    case StateMachine::State::Play:
    {
        // Setup for common scene
        MCGLScene & glScene = MCWorld::instance().renderer().glScene();
        glScene.setSplitType(MCGLScene::ShowFullScreen);

        if (m_race.checkeredFlagEnabled() && !m_game.hasTwoHumanPlayers())
        {
            m_checkeredFlag->render();
        }

        m_startlightsOverlay->render();
        m_messageOverlay->render();
        break;
    }
    default:
        break;
    };
}

void Scene::renderHUD()
{
    switch (m_stateMachine.state())
    {
    case StateMachine::State::GameTransitionIn:
    case StateMachine::State::GameTransitionOut:
    case StateMachine::State::DoStartlights:
    case StateMachine::State::Play:
    {
        MCGLScene & glScene = MCWorld::instance().renderer().glScene();

        if (m_game.hasTwoHumanPlayers())
        {
            MCGLScene::SplitType p1, p0;
            getSplitPositions(p1, p0);

            glScene.setSplitType(p1);
            m_timingOverlay[1].render();
            m_minimap[1].render(m_cars, m_race);
            m_crashOverlay[1].render();

            glScene.setSplitType(p0);
            m_timingOverlay[0].render();
            m_minimap[0].render(m_cars, m_race);
            m_crashOverlay[0].render();

            glScene.setSplitType(MCGLScene::ShowFullScreen);
        }
        else
        {
            m_timingOverlay[0].render();
            m_minimap[0].render(m_cars, m_race);
            m_crashOverlay[0].render();
        }

        break;
    }
    default:
        break;
    };
}

void Scene::renderWorld(MCRenderGroup renderGroup, bool prepareRendering)
{
    switch (m_stateMachine.state())
    {
    case StateMachine::State::GameTransitionIn:
    case StateMachine::State::GameTransitionOut:
    case StateMachine::State::DoStartlights:
    case StateMachine::State::Play:
    {
        MCGLScene & glScene = MCWorld::instance().renderer().glScene();

        if (m_game.hasTwoHumanPlayers())
        {
            MCGLScene::SplitType p1, p0;
            getSplitPositions(p1, p0);

            if (prepareRendering)
            {
                m_world.prepareRendering(&m_camera[1]);
                m_world.prepareRendering(&m_camera[0]);
            }

            glScene.setSplitType(p1);
            m_world.render(&m_camera[1], renderGroup);

            glScene.setSplitType(p0);
            m_world.render(&m_camera[0], renderGroup);

            glScene.setSplitType(MCGLScene::ShowFullScreen);
        }
        else
        {
            if (prepareRendering)
            {
                m_world.prepareRendering(&m_camera[0]);
            }

            m_world.render(&m_camera[0], renderGroup);
        }

        break;
    }
    default:
        break;
    };
}

Scene::~Scene()
{
    delete m_fadeAnimation;
    delete m_intro;
    delete m_menuManager;
    delete m_messageOverlay;
    delete m_particleFactory;
    delete m_startlights;
    delete m_startlightsOverlay;
}
