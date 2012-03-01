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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "car.hpp"
#include "race.hpp"

#include <vector>

class AiLogic;
class InputHandler;
class MCCamera;
class MCObject;
class MCSurface;
class MCWorld;
class OffTrackDetector;
class Speedometer;
class Startlights;
class StartlightsOverlay;
class Track;
class TimingOverlay;

//! The game scene.
class Scene
{
public:

    //! Constructor.
    explicit Scene(MCUint numCars = 8);

    //! Destructor.
    ~Scene();

    //! Update physics and objects by the given
    //! time step.
    void updateFrame(InputHandler & handler,
        MCCamera & camera, float timeStep);

    //! Set the active race track.
    void setActiveTrack(Track & activeTrack);

    //! Return the active race track.
    Track & activeTrack() const;

    //! Return the world.
    MCWorld & world() const;

    //! Set timing overlay.
    void setTimingOverlay(TimingOverlay & timingOverlay);

    //! Set speedometer.
    void setSpeedometer(Speedometer & speedometer);

    //! Set startlights.
    void setStartlights(Startlights & startlights);

    //! Set startlightsOverlay.
    void setStartlightsOverlay(StartlightsOverlay & startlightsOverlay);

    //! Get timing overlay.
    TimingOverlay & timingOverlay() const;

    //! Render all components.
    void render(MCCamera & camera);

private:

    void updateWorld(float timeStep);

    void updateRace();

    void updateCameraLocation(MCCamera & camera);

    void processUserInput(InputHandler & handler);

    void updateAiLogic();

    void setWorldDimensions();

    void addTrackObjectsToWorld();

    void addCarsToWorld();

    void translateCarsToStartPositions();

    void initRace();

    Race                 m_race;
    Track              * m_pActiveTrack;
    MCWorld            * m_pWorld;
    TimingOverlay      * m_pTimingOverlay;
    Speedometer        * m_pSpeedometer;
    Startlights        * m_pStartlights;
    StartlightsOverlay * m_pStartlightsOverlay;
    MCFloat              m_cameraBaseOffset;

    typedef std::vector<Car *> CarVector;
    CarVector m_cars;

    typedef std::vector<AiLogic *> AiVector;
    AiVector m_aiLogic;

    typedef std::vector<OffTrackDetector *> OTDVector;
    OTDVector m_offTrackDetectors;
};

#endif // SCENE_HPP
