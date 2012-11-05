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

#include "car.hpp"
#include "race.hpp"
#include <MCCamera>

#include <memory>
#include <vector>

class AI;
class CheckeredFlag;
class Game;
class InputHandler;
class Intro;
class MCCamera;
class MCObject;
class MCSurface;
class MCWorld;
class MessageOverlay;
class OffTrackDetector;
class Renderer;
class Startlights;
class StartlightsOverlay;
class StateMachine;
class Track;
class TrackSelectionMenu;
class TimingOverlay;

namespace MTFH {
class Menu;
class MenuManager;
}

//! The game scene.
class Scene
{
public:

    //! Constructor.
    Scene(
        Game & game, StateMachine & stateMachine, Renderer & renderer, unsigned int numCars = 10);

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

    //! Update physics and objects by the given time step.
    void updateFrame(InputHandler & handler, float timeStep);

    //! Update animations.
    void updateAnimations();

    //! Set the active race track.
    void setActiveTrack(Track & activeTrack);

    //! Return the active race track.
    Track & activeTrack() const;

    //! Return the world.
    MCWorld & world() const;

    //! Return track selection menu.
    TrackSelectionMenu & trackSelectionMenu() const;

    //! Return camera.
    MCCamera & camera()
    {
        return m_camera;
    }

    //! Render all components.
    void render(MCCamera & camera);

private:

    void addCarsToWorld();
    void addTrackObjectsToWorld();
    void createMenus();
    void initRace();
    void processUserInput(InputHandler & handler, bool isRaceCompleted);
    void renderCommonScene(MCCamera & camera);
    void renderPlayerScene(MCCamera & camera);
    void setWorldDimensions();
    void translateCarsToStartPositions();
    void updateAI();
    void updateCameraLocation(MCCamera & camera);
    void updateRace();
    void updateWorld(float timeStep);

    static int            m_width;
    static int            m_height;
    Game                & m_game;
    StateMachine        & m_stateMachine;
    Renderer            & m_renderer;
    MessageOverlay      * m_messageOverlay;
    Race                  m_race;
    Track               * m_activeTrack;
    MCWorld             * m_world;
    MCCamera              m_camera;
    TimingOverlay       * m_timingOverlay;
    Startlights         * m_startlights;
    StartlightsOverlay  * m_startlightsOverlay;
    CheckeredFlag       * m_checkeredFlag;
    MCFloat               m_cameraBaseOffset;
    TrackSelectionMenu  * m_trackSelectionMenu;
    MTFH::Menu          * m_mainMenu;
    MTFH::Menu          * m_help;
    MTFH::Menu          * m_credits;
    MTFH::Menu          * m_settings;
    MTFH::MenuManager   * m_menuManager;
    Intro               * m_intro;

    typedef std::shared_ptr<Car> CarPtr;
    typedef std::vector<CarPtr> CarVector;
    CarVector m_cars;

    typedef std::shared_ptr<AI> AIPtr;
    typedef std::vector<AIPtr> AIVector;
    AIVector m_ai;

    typedef std::shared_ptr<OffTrackDetector> OffTrackDetectorPtr;
    typedef std::vector<OffTrackDetectorPtr> OTDVector;
    OTDVector m_offTrackDetectors;
};

#endif // SCENE_HPP
