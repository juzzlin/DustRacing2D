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
#include "credits.hpp"
#include "fadeanimation.hpp"
#include "game.hpp"
#include "inputhandler.hpp"
#include "intro.hpp"
#include "help.hpp"
#include "layers.hpp"
#include "mainmenu.hpp"
#include "messageoverlay.hpp"
#include "particlefactory.hpp"
#include "pit.hpp"
#include "race.hpp"
#include "renderer.hpp"
#include "settings.hpp"
#include "settingsmenu.hpp"
#include "startlights.hpp"
#include "startlightsoverlay.hpp"
#include "statemachine.hpp"
#include "timingoverlay.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackobject.hpp"
#include "trackselectionmenu.hpp"
#include "tracktile.hpp"
#include "treeview.hpp"

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
#include <MCObject>
#include <MCPhysicsComponent>
#include <MCShape>
#include <MCSurface>
#include <MCSurfaceView>
#include <MCTextureFont>
#include <MCTypes>
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

static const MCFloat METERS_PER_UNIT = 0.05f;

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
, m_trackSelectionMenu(nullptr)
, m_mainMenu(nullptr)
, m_help(nullptr)
, m_credits(nullptr)
, m_settingsMenu(nullptr)
, m_menuManager(nullptr)
, m_intro(new Intro)
, m_particleFactory(new ParticleFactory)
, m_fadeAnimation(new FadeAnimation)
{
    connect(m_startlights, SIGNAL(raceStarted()), &m_race, SLOT(start()));
    connect(m_startlights, SIGNAL(animationEnded()), &m_stateMachine, SLOT(endStartlightAnimation()));

    connect(&m_stateMachine, SIGNAL(startlightAnimationRequested()), m_startlights, SLOT(beginAnimation()));
    connect(&m_stateMachine, SIGNAL(fadeInRequested(int, int, int)), m_fadeAnimation, SLOT(beginFadeIn(int, int, int)));
    connect(&m_stateMachine, SIGNAL(fadeOutRequested(int, int, int)), m_fadeAnimation, SLOT(beginFadeOut(int, int, int)));
    connect(&m_stateMachine, SIGNAL(fadeOutFlashRequested(int, int, int)), m_fadeAnimation, SLOT(beginFadeOutFlash(int, int, int)));
    connect(&m_stateMachine, SIGNAL(soundsStopped()), &m_race, SLOT(stopEngineSounds()));

    connect(m_fadeAnimation, SIGNAL(fadeValueChanged(float)), &m_renderer, SLOT(setFadeValue(float)));
    connect(m_fadeAnimation, SIGNAL(fadeInFinished()), &m_stateMachine, SLOT(endFadeIn()));
    connect(m_fadeAnimation, SIGNAL(fadeOutFinished()), &m_stateMachine, SLOT(endFadeOut()));

    connect(&m_race, SIGNAL(finished()), &m_stateMachine, SLOT(finishRace()));
    connect(&m_race, SIGNAL(messageRequested(QString)), m_messageOverlay, SLOT(addMessage(QString)));

    connect(m_startlights, SIGNAL(messageRequested(QString)), m_messageOverlay, SLOT(addMessage(QString)));
    connect(this, SIGNAL(listenerLocationChanged(float, float)), &m_game.audioWorker(), SLOT(setListenerLocation(float, float)));

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

    const MCGLAmbientLight ambientLight(1.0, 0.9, 0.95, 0.7);
    const MCGLDiffuseLight diffuseLight(MCVector3dF(1.0, -1.0, -1.0), 1.0, 0.9, 0.85, 0.3);
    const MCGLDiffuseLight specularLight(MCVector3dF(1.0, -1.0, -1.0), 1.0, 1.0, 1.0, 1.0);

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

    m_mainMenu = new MainMenu("main", width(), height());
    connect(
        static_cast<MainMenu *>(m_mainMenu), SIGNAL(exitGameRequested()), &m_game, SLOT(exitGame()));
    m_menuManager->addMenu(*m_mainMenu);

    m_help = new Help("help", width(), height());
    m_menuManager->addMenu(*m_help);

    m_credits = new Credits("credits", width(), height());
    m_menuManager->addMenu(*m_credits);

    m_settingsMenu = new SettingsMenu("settings", width(), height());
    m_menuManager->addMenu(*m_settingsMenu);

    m_trackSelectionMenu = new TrackSelectionMenu("trackSelection", width(), height(), *this);
    m_menuManager->addMenu(*m_trackSelectionMenu);

    m_menuManager->enterMenu(*m_mainMenu);
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

void Scene::updateAnimations()
{
    for (auto && car : m_cars)
    {
        car->update();
    }

    for (auto && treeView : m_treeViews)
    {
        treeView->update();
    }
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

void Scene::updateCameraLocation(MCCamera & camera, MCFloat & offset, MCObject & object)
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
    const MCUint minX = 0;
    const MCUint maxX = m_activeTrack->width();
    const MCUint minY = 0;
    const MCUint maxY = m_activeTrack->height();
    const MCUint minZ = 0;
    const MCUint maxZ = 1000;

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

    m_treeViews.clear();

    for (unsigned int i = 0; i < m_activeTrack->trackData().objects().count(); i++)
    {
        auto trackObject = dynamic_pointer_cast<TrackObject>(m_activeTrack->trackData().objects().object(i));
        assert(trackObject);

        MCObject & mcObject = trackObject->object();
        mcObject.addToWorld();
        mcObject.translate(mcObject.initialLocation());
        mcObject.rotate(mcObject.initialAngle());

        if (auto treeView = dynamic_pointer_cast<TreeView>(mcObject.shape()->view()))
        {
            m_treeViews.push_back(treeView);
        }
        else if (auto pit = dynamic_cast<Pit *>(&mcObject))
        {
            connect(pit, SIGNAL(pitStop(Car &)), &m_race, SLOT(pitStop(Car &)));
        }
    }
}

void Scene::createBridgeObjects()
{
    assert(m_activeTrack);

    const MapBase & rMap = m_activeTrack->trackData().map();

    static const int w = TrackTile::TILE_W;
    static const int h = TrackTile::TILE_H;

    for (MCUint j = 0; j <= rMap.rows(); j++)
    {
        for (MCUint i = 0; i <= rMap.cols(); i++)
        {
            auto tile = dynamic_pointer_cast<TrackTile>(rMap.getTile(i, j));
            if (tile && tile->tileTypeEnum() == TrackTile::TT_BRIDGE)
            {
                MCObjectPtr bridge(new Bridge(
                    MCAssetManager::instance().surfaceManager().surface("bridgeObject"),
                    MCAssetManager::instance().surfaceManager().surface("wallLong")
                ));

                bridge->translate(MCVector3dF(i * w + w / 2, j * h + h / 2, Bridge::zOffset()));
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

TrackSelectionMenu & Scene::trackSelectionMenu() const
{
    assert(m_trackSelectionMenu);
    return *m_trackSelectionMenu;
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
    const MCFloat fadeValue = m_renderer.fadeValue();

    switch (m_stateMachine.state())
    {
    case StateMachine::State::GameTransitionIn:
    case StateMachine::State::GameTransitionOut:
    case StateMachine::State::DoStartlights:
    case StateMachine::State::Play:
    {
        MCGLScene & glScene = MCWorld::instance().renderer().glScene();
        if (m_fadeAnimation->isFading())
        {
            // TODO glScene.setFadeValue(fadeValue);
        }

        if (m_game.hasTwoHumanPlayers())
        {
            MCGLScene::SplitType p1, p0;
            getSplitPositions(p1, p0);

            glScene.setSplitType(p1);
            m_activeTrack->render(&m_camera[1]);

            glScene.setSplitType(p0);
            m_activeTrack->render(&m_camera[0]);

            // Setup for common scene
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

void Scene::renderObjectShadows()
{
    const MCFloat fadeValue = m_renderer.fadeValue();

    switch (m_stateMachine.state())
    {
    case StateMachine::State::GameTransitionIn:
    case StateMachine::State::GameTransitionOut:
    case StateMachine::State::DoStartlights:
    case StateMachine::State::Play:
    {
        MCGLScene & glScene = MCWorld::instance().renderer().glScene();

        if (m_fadeAnimation->isFading())
        {
            // TODO glScene.setFadeValue(fadeValue);
        }

        if (m_game.hasTwoHumanPlayers())
        {
            MCGLScene::SplitType p1, p0;
            getSplitPositions(p1, p0);

            glScene.setSplitType(p1);
            renderPlayerSceneShadows(m_camera[1]);

            glScene.setSplitType(p0);
            renderPlayerSceneShadows(m_camera[0]);

            // Setup for common scene
            glScene.setSplitType(MCGLScene::ShowFullScreen);
        }
        else
        {
            renderPlayerSceneShadows(m_camera[0]);
        }

        break;
    }
    default:
        break;
    };
}

void Scene::renderObjects()
{
    const MCFloat fadeValue = m_renderer.fadeValue();
    MCGLScene & glScene = MCWorld::instance().renderer().glScene();

    switch (m_stateMachine.state())
    {
    case StateMachine::State::DoIntro:

        glScene.setSplitType(MCGLScene::ShowFullScreen);
        m_intro->setFadeValue(fadeValue);
        m_intro->render();

        break;

    case StateMachine::State::Menu:
    case StateMachine::State::MenuTransitionOut:
    case StateMachine::State::MenuTransitionIn:

        // TODO glScene.setFadeValue(fadeValue);
        glScene.setSplitType(MCGLScene::ShowFullScreen);

        m_menuManager->render();

        break;

    case StateMachine::State::GameTransitionIn:
    case StateMachine::State::GameTransitionOut:
    case StateMachine::State::DoStartlights:
    case StateMachine::State::Play:

        if (m_fadeAnimation->isFading())
        {
            // TODO glScene.setFadeValue(fadeValue);
        }

        if (m_game.hasTwoHumanPlayers())
        {
            MCGLScene::SplitType p1, p0;
            getSplitPositions(p1, p0);

            glScene.setSplitType(p1);
            renderPlayerScene(m_camera[1]);
            m_timingOverlay[1].render();
            m_minimap[1].render(m_cars, m_race);

            glScene.setSplitType(p0);
            renderPlayerScene(m_camera[0]);
            m_timingOverlay[0].render();
            m_minimap[0].render(m_cars, m_race);


            // Setup for common scene
            glScene.setSplitType(MCGLScene::ShowFullScreen);
        }
        else
        {
            renderPlayerScene(m_camera[0]);
            m_timingOverlay[0].render();
            m_crashOverlay[0].render();

            m_minimap[0].render(m_cars, m_race);
        }

        break;

    default:
        break;
    };
}

void Scene::renderPlayerScene(MCCamera & camera)
{
    // Assume that m_world.prepareRendering(&camera) is already called.
    m_world.render(&camera);
}

void Scene::renderPlayerSceneShadows(MCCamera & camera)
{
    m_world.prepareRendering(&camera);
    m_world.renderShadows(&camera);
}

void Scene::renderCommonHUD()
{
    switch (m_stateMachine.state())
    {
    case StateMachine::State::GameTransitionIn:
    case StateMachine::State::GameTransitionOut:
    case StateMachine::State::DoStartlights:
    case StateMachine::State::Play:
        if (m_race.checkeredFlagEnabled() && !m_game.hasTwoHumanPlayers())
        {
            m_checkeredFlag->render();
        }

        m_startlightsOverlay->render();
        m_messageOverlay->render();
        break;
    default:
        break;
    };
}

Scene::~Scene()
{
    delete m_credits;
    delete m_fadeAnimation;
    delete m_help;
    delete m_intro;
    delete m_mainMenu;
    delete m_menuManager;
    delete m_messageOverlay;
    delete m_particleFactory;
    delete m_settingsMenu;
    delete m_startlights;
    delete m_startlightsOverlay;
    delete m_trackSelectionMenu;
}
