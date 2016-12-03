// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCSEGMENT_HH
#define MCSEGMENT_HH

#include "mcvector2d.hh"

//! MCSegment is a structure for a geometrical segment.
template <typename T>
struct MCSegment
{
    //! Constructor.
    MCSegment(const MCVector2d<T> & v0, const MCVector2d<T> & v1)
    : vertex0(v0)
    , vertex1(v1)
    {}

    //! Constructor.
    MCSegment()
    {}

    MCVector2d<T> vertex0;
    MCVector2d<T> vertex1;
};

typedef MCSegment<MCFloat> MCSegmentF;

#endif // MCSEGMENT_HH
