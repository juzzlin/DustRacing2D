// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "ai.hpp"
#include "car.hpp"
#include "crashoverlay.hpp"
#include "minimap.hpp"
#include "race.hpp"
#include "timingoverlay.hpp"

#include <MCCamera>
#include <QObject>
#include <memory>
#include <vector>

#include <MenuManager>

class CheckeredFlag;
class FadeAnimation;
class Game;
class InputHandler;
class Intro;
class MCCamera;
class MCObject;
class MCSurface;
class MCWorld;
class MessageOverlay;
class ParticleFactory;
class Renderer;
class Startlights;
class StartlightsOverlay;
class StateMachine;
class Track;
class TrackSelectionMenu;

namespace MTFH {
class Menu;
}

//! The game scene.
class Scene : public QObject
{
    Q_OBJECT

public:
    static const int NUM_CARS = 12;

    //! Constructor.
    Scene(Game & game, StateMachine & stateMachine, Renderer & renderer, MCWorld & world);

    //! Destructor.
    ~Scene();

    //! Width of the scene. This is always constant and doesn't
    //! depend on resolution.
    static int width();

    //! Height of the scene. This is always constant and doesn't
    //! depend on resolution.
    static int height();

    //! Set scene size.
    static void setSize(int width, int height);

    //! Update physics and objects by the given time step in ms.
    void updateFrame(InputHandler & handler, int step);

    //! Update HUD overlays.
    void updateOverlays();

    //! Set the active race track.
    void setActiveTrack(Track & activeTrack);

    //! Return the active race track.
    Track & activeTrack() const;

    //! Return track selection menu.
    MTFH::MenuPtr trackSelectionMenu() const;

    void renderCommonHUD();

    void renderHUD();

    void renderMenu();

    void renderTrack();

    void renderWorld(MCRenderGroup renderGroup, bool prepareRendering = false);

signals:

    void listenerLocationChanged(float x, float y);

private:
    void addCarsToWorld();

    void addTrackObjectsToWorld();

    void connectComponents();

    void createBridgeObjects();

    void createCars();

    void createMenus();

    void createNormalObjects();

    void initializeComponents();

    void initializeRace();

    void processUserInput(InputHandler & handler);

    void renderPlayerScene(MCCamera & camera);

    void renderPlayerSceneShadows(MCCamera & camera);

    void resizeOverlays();

    void setupAudio(Car & car, int index);

    void setupAI(Track & activeTrack);

    void setupCameras(Track & activeTrack);

    void setupMinimaps();

    void getSplitPositions(MCGLScene::SplitType & p0, MCGLScene::SplitType & p1);

    void setWorldDimensions();

    void updateAi();

    void updateCameraLocation(MCCamera & camera, float & offset, MCObject & object);

    void updateRace();

    void updateWorld(int timeStep);

    static int m_width;

    static int m_height;

    Game & m_game;

    StateMachine & m_stateMachine;

    Renderer & m_renderer;

    std::unique_ptr<MessageOverlay> m_messageOverlay;

    Race m_race;

    Track * m_activeTrack;

    MCWorld & m_world;

    CrashOverlay m_crashOverlay[2];

    TimingOverlay m_timingOverlay[2];

    std::unique_ptr<Startlights> m_startlights;

    std::unique_ptr<StartlightsOverlay> m_startlightsOverlay;

    std::unique_ptr<CheckeredFlag> m_checkeredFlag;

    MCCamera m_camera[2];

    float m_cameraOffset[2];

    MTFH::MenuPtr m_mainMenu;

    std::unique_ptr<MTFH::MenuManager> m_menuManager;

    std::unique_ptr<Intro> m_intro;

    std::unique_ptr<ParticleFactory> m_particleFactory;

    std::unique_ptr<FadeAnimation> m_fadeAnimation;

    Minimap m_minimap[2];

    using CarVector = std::vector<CarPtr>;
    CarVector m_cars;

    using AIVector = std::vector<AIPtr>;
    AIVector m_ai;

    // Bridges
    std::vector<MCObjectPtr> m_bridges;
};

#endif // SCENE_HPP
