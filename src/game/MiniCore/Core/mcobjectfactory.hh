// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCOBJECTFACTORY_HH
#define MCOBJECTFACTORY_HH

#include "mcsurfaceobjectdata.hh"
#include "mcglobjectdata.hh"
#include "mcobject.hh"

class MCObjectFactoryImpl;
class MCTextureManager;

//! MCObjectFactory is a factory class that can be used when loading objects from file.
class MCObjectFactory
{
public:

    //! Constructor.
    MCObjectFactory(MCTextureManager & tm);

    //! Destructor
    virtual ~MCObjectFactory();

    //! Build an object with a surface view.
    //! MCObjectFactory keeps the ownership.
    MCObject & build(const MCSurfaceObjectData & data);

    //! Build an object with a GL view.
    //! MCObjectFactory keeps the ownership.
    MCObject & build(const MCGLObjectData & data);

    //! Build an object with a custom view.
    //! MCObjectFactory keeps the ownership.
    MCObject & build(const MCObjectData & data, MCShapeView & view);

private:

    DISABLE_COPY(MCObjectFactory);
    DISABLE_ASSI(MCObjectFactory);
    MCObjectFactoryImpl * const m_pImpl;
    friend class MCObjectFactoryImpl;
};

#endif // MCOBJECTFACTORY_HH
