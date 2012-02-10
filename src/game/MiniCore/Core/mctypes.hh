// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2009 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA.
//

#ifndef MCTYPES_HH
#define MCTYPES_HH

#include "mcvector2d.hh"
#include "mcvector3d.hh"

// Floating point data type used
#ifdef MC_USE_DOUBLE
  typedef double MCFloat;
#else
  typedef float MCFloat;
#endif

// Some convenience typedef's
typedef unsigned int  MCUint;
typedef unsigned char MCUchar;

typedef MCVector3d<MCFloat> MCVector3dF;
typedef MCVector2d<MCFloat> MCVector2dF;

typedef const MCVector3dF & MCVector3dFR;
typedef const MCVector2dF & MCVector2dFR;

#endif // MCTYPES_HH
