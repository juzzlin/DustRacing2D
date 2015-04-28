1.11.0
======

New features:

* Finally a difficulty setting with three levels: Easy, Medium and Senna
* Show a red overlay on hard crashes
* Cars can get damaged

Improvements:

* Remove alpha blending from the plant object
* Stabilize the steering a bit

Bug fixes:

* Editor: Fix wrong scale when track given as a CLI argument
* Editor: Don't try to open a track if filename is empty
* Fix unit tests

Other:

* Refactor settingsmenu.hpp
* Refactor steering logic
* Sparkles as surface particles
* Use same renderer for all particle batches

1.10.0
======

In this release multiple issues with shadow rendering are fixed.
Particle rendering is completely reworked and also the track editor
is given some love. Issues with depth testing are fixed. A lot of
bug fixes and miscellaneous improvements and changes.

New features:

* Add a chicane to straight.trk and make it shorter
* Instantly change the virtual resolution between fullscreen modes
* Longer start grid to Suzuka
* Make it possible to spin
* Show lap time in message area
* Simple specular effect for the cars
* Take more car colors in to use
* Editor: Different colors for brake and brake hard
* Editor: Implement row and col deletion
* Editor: Implement row and column insertion

Improvements:

* Enable depth test for smoke
* Implement batching and shadows for MCSurfaceParticles
* Make timing based on frames instead of QTime
* Mud particles as surface particles
* No leaf particle when colliding with plants
* Reduce quadratic drag
* Refactor particle rendering
* Rename textures.conf => surfaces.conf
* Render rect particles as quads if not GLES
* Render shadows with depth information
* Set z-offsets for car surfaces
* Smaller tree swing amplitude
* Use GLSL's reflect()
* Editor: Fit track properly when loading a track
* Editor: Improve some tile graphics
* Editor: Replace private slots with lambdas in EditorView
* Editor: Replace some if's with asserts
* Editor: Use C++11's override specifier

Bug fixes:

* Fix a bug where mouse release outside pressed item caused two items being focused
* Fix build with qmake
* Fix cppcheck warnings
* Fix depth buffering
* Fix exception handling
* Fix specular lighting of the tire object

Other:

* Add optional z attribute to MCSurface / surface config loader
* Add mcvector2d.hh and mcvector3d.hh to CMakeLists.txt
* Add option to filter layers to be rendered
* C++11 way to disable copy ctor's
* Common base class for all particle renderers
* Divide layers into render and collision layers
* Lambda slots for show/hide cursor
* Lambda slot for the update timer
* More sane use of casts
* Move MCGLScene to MCWorldRenderer
* New-style signal connections in Game
* Refactor bridge creation
* Refactor Config
* Refactor child velocity logic
* Refactor MCSurfaceConfigLoader
* Remove obsolete images
* Remove render layers from cars' sub-objects
* Replace MCException with std::runtime_error
* Use add_compile_options() instead of appending CMAKE_CXX_FLAGS
* Use C++11's move semantics in MCBBox and MCBBox3d
* Use C++11's move semantics in MCVector2d and MCVector3d

1.9.2
=====

Bug fixes:

* Fix rendering of the windowed mode.
* Fix rendering in other than native resolutions.

Improvements:

* Take maximum supported texture size into account.
* QGLFramebufferObject => QOpenGLFramebufferObject.
* Add camera location support to MCGLShaderProgram.
* Simplify FBO shading.
* CMake: Check for polices before setting.
* Fix some MSVC compilation warnings.
* Improve documentaton in mcglscene.hh
* Add getters for eyeZ and viewAngle.

1.9.1
=====

New features:

* Rotating leaf particles
* Show game version in the intro screen.

Improvements:

* Set VSync default to 'ON'.
* Force QSettings to .INI-format on Windows.
* Make the code compile with MS Visual C++ 2013.
* Improved tile graphics
* Re-license artwork under CC BY-SA
* Switch to the new QSurface-based graphics stack
* Engine improvements

Bug fixes:

* Fix broken OpenGL scene size on virtual desktops.
* Fix multiple bridge issues
* Fix crash when quitting
* Fix rendering of two-player game

1.8.3
=====

Bug fixes:

* Fix some physics issues

1.8.2
=====

New features:

* New race track suzuka.trk

Improvements:

* Physics improvements
  - More stable and agile car handling
  - Lighter tire object

Bug fixes:

* Fix multiple bugs in the bridge component
* Fix compilation with Clang

1.8.1
=====

Improvements:

* Tune car parameters

Other:

* Drop support for Qt4

1.8.0
=====

New features:

* More realistic and dynamic car physics for funnier gameplay.
* Rewritten bridge object that now actually even works
* Some graphics improvements (they are still shitty, though)
  - Transparent shadows
  - Two-player game views separated by a narrow line
  - Filtering added to some textures
  - Some car graphics improved
* Driving off-track is now more merciful

Bug fixes:

* Fix installing the game when not built in the source root. Patch by Rémi Verschelde.
* Enhancement of the desktop files. Patch by Rémi Verschelde.
* Fix bridge

Other:

* Improve target zones of ring.trk

1.7.3
=====

Maintenance release:

* Revert "Optimize VBO, VAO and texture binding."
  - Caused missing number plate of the first car
* Fix MCObject::addTorque()
* Remove linear and angular velocity limits.
* Add support for positional force.
* Fix build with GL30=ON.
* Enable build on FreeBSD.

1.7.2
=====

New features

* French translations

Bug fixes:

* Fix computer driving lines in twister.trk

1.7.1
=====

Bug fixes:

* Add missing bridge model for editor.

1.7.0
=====

New features:

* twister.trk
* Bridge object.
* Fade out flash transition.
* Turn left and turn right signs.
* CLI option to force vsync off.
* New crash sound.
* Editor: visualize computer hints.
* Move user tracks to the beginning of the track array.
* Increase the initial slide friction to 1.0

Bug fixes:

* Don't show prev/next if on first/last item.
* Remove specular shaders from crate and tire.
* Fix computer driving line in curvastone.trk
* Fix off-track detection on 'sand grass straight' tile.

Other:

* Small improvements to render timing.
* Optimize VBO, VAO and texture binding.
* Optimize viewport switching.
* Optimize the use of GL_DEPTH_TEST.
* Use random functionality from C++11.
* Improve sky reflection.
* Update scripts/genericLinuxBuildQt5

-- Jussi Lind <jussi.lind@iki.fi>

