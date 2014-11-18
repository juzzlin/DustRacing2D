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

