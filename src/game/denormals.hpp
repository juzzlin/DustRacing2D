// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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
#ifndef DENORMALS_HPP
#define DENORMALS_HPP

namespace Denormals {

/*! Flushes denormal floating point numbers to zero on the calling thread. Calculations with
 *  denormals are very slow on many CPUs, which causes load spikes e.g. when velocities in the
 *  physics decay towards zero. This is what -ffast-math does at startup, but without its other
 *  effects on floating point math. Must be called on every thread that does heavy math. */
void disable();

} // namespace Denormals

#endif // DENORMALS_HPP
