## Dust Racing 2D

<table>
  <tr>
    <td>
      <img src="https://github.com/juzzlin/DustRacing2D/blob/gh-pages/screenshots/1.13.0/1.jpg" width="100%"></img>
    </td>
    <td>
      <img src="https://github.com/juzzlin/DustRacing2D/blob/gh-pages/screenshots/1.13.0/3.jpg" width="100%"></img>
    </td>
  </tr>
</table>

<a href="https://www.youtube.com/watch?feature=player_embedded&v=3hrIzzItggQ">Video of the game</a>

Dust Racing 2D (Dustrac) is a tile-based, cross-platform 2D racing game written
in Qt (C++) and OpenGL.

Dust Racing 2D comes with a Qt-based level editor for easy level creation.
A separate engine, MiniCore, is used for physics modeling.

## Features

* 1-2 human players againts 11 challenging computer players
* 3 difficulty settings: Easy, Medium, Hard
* Split-screen two player game (vertical or horizontal)
* Game modes: Race, Time Trial, Duel
* 2D graphics with some 3D objects
* Smooth game play and physics
* Multiple race tracks
* Finishing in TOP-6 will unlock the next race track
* Star ratings based on the best positions on each race track
* Easy to create new race tracks with the level editor
* Engine and collision sounds
* Pit stops
* Runs windowed or fullscreen
* Portable source code using CMake as the build system
* Will be forever completely free

## License

Dust Racing 2D source code is licensed under GNU GPLv3.
See COPYING for the complete license text.

Dust Racing 2D includes the source code for the GLEW library.
See src/game/MiniCore/Graphics/glew/glew.h for the license text.

Dust Racing 2D includes the source code for the GLM library.
See src/game/MiniCore/Graphics/glm/ogl-math/glm/glm.hpp for the license text.

All image files, except where otherwise noted, are licensed under
CC BY-SA 3.0: http://creativecommons.org/licenses/by-sa/3.0/

## Playing

### Controls

The default controls for player one (the pink race car):

Accelerate | Brake | Turn left | Turn right
---------- | ----- | --------- | ----------
Up         | Down  | Left      | Right

The default controls for player two (the grey race car):

Accelerate | Brake | Turn left | Turn right
---------- | ----- | --------- | ----------
W          | S     | A         | D

The key configuration and game mode can be changed in the settings menu.

**ESC** or **Q** exits the race and also the current menu.

**P** pauses the game.

### Races

In the race modes there are always 12 cars. By finishing in TOP-6 a new track
will be unlocked.

The record times and best positions are stored separately for each lap count.

### Pit stops

Your tires will wear out as the race progresses. This causes more and more sliding.

Fortunately there's a pit (the yellow rectangle).
By stopping on the pit your tires will be repaired.

### Custom track files

Dust Racing 2D searches for race tracks in `~/DustRacingTracks/` where you can place your own race tracks.

On Linux/Unix also `$XDG_DATA_HOME/DustRacing2D/tracks` is added to search paths, or `$HOME/.local/share/DustRacing2D/tracks` if `$XDG_DATA_HOME` is not defined.

## Command line options

`--lang [lang]` forces the language. Currently available: `fi`, `cs`, `de`, `it`, `fr`, `nl`, `ru`, `tr`.

E.g. `dustrac-game --lang it`

`--screen [index]` forces the screen on multi-display setups when in fullscreen mode.

E.g. `dustrac-game --screen 1` would start the game on the second display.

## Building the project

Please refer to the `INSTALL` document for build/install instructions if you're
going to build **Dust Racing 2D** from sources.

In a nutshell, on Linux after installing `Qt5`, `OpenAL` and `Vorbis` dev libs you just:

`$ mkdir build && cd build`

`$ cmake ..`

`$ make`

..or open the top-level `CMakeLists.txt` in **Qt Creator** or other IDE that supports CMake.
