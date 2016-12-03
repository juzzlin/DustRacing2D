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

#ifndef MCTRIGONOM_HH
#define MCTRIGONOM_HH

#include "mctypes.hh"
#include "mcmacros.hh"
#include "mcvector2d.hh"

#include <memory>

class MCTrigonomImpl;

/*!
 *  \class MCTrigonom
 *
 *  MCTrigonom implements fast trigonometry routines using cached values from
 *  lookup tables.
 *
 *  Cannot be instantiated.
 */
class MCTrigonom
{
public:

    //! Convert degree to radians
    static MCFloat degToRad(MCFloat angle);

    //! Convert radian to degrees
    static MCFloat radToDeg(MCFloat angle);

    //! Get sine of given angle (0..360)
    static MCFloat sin(MCFloat angle);

    //! Get cosine of given angle (0..360)
    static MCFloat cos(MCFloat angle);

private:

    //! Disabled constructor.
    MCTrigonom();

    DISABLE_COPY(MCTrigonom);
    DISABLE_ASSI(MCTrigonom);

    static std::unique_ptr<MCTrigonomImpl> const m_pImpl;
};

#endif // MCTRIGONOM_HH

