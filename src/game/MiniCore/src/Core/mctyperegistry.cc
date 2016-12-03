// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include "mctyperegistry.hh"

MCTypeRegistry::MCTypeRegistry()
    : m_typeIDCount(1)
{
}

MCUint MCTypeRegistry::registerType(const std::string & typeName)
{
    auto i(m_typeHash.find(typeName));
    if (i == m_typeHash.end())
    {
        m_typeIDCount++;
        m_typeHash[typeName] = m_typeIDCount;
        return m_typeIDCount;
    }
    else
    {
        return i->second;
    }
}

MCUint MCTypeRegistry::getTypeIDForName(const std::string & typeName)
{
    auto i(m_typeHash.find(typeName));
    return i == m_typeHash.end() ? 0 : i->second;
}
