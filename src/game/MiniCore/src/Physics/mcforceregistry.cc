// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcobject.hh"
#include "mcforceregistry.hh"

#include <algorithm>

MCForceRegistry::MCForceRegistry()
: m_registryHash()
{}

void MCForceRegistry::update()
{
    auto iter = m_registryHash.begin();
    while (iter != m_registryHash.end())
    {
        Registry & registry = iter->second;
        for (MCUint i = 0; i < registry.size(); i++)
        {
            if (iter->first->index() != -1)
            {
                if (registry[i]->enabled())
                {
                    registry[i]->updateForce(*iter->first);
                }
            }
        }

        iter++;
    }
}

void MCForceRegistry::addForceGenerator(MCForceGeneratorPtr generator, MCObject & object)
{
    MCForceRegistry::Registry & registry = m_registryHash[&object];
    if (std::find(registry.begin(), registry.end(), generator) == registry.end())
    {
        registry.push_back(generator);
    }
}

void MCForceRegistry::removeForceGenerator(MCForceGeneratorPtr generator, MCObject & object)
{
    auto iter = m_registryHash.find(&object);
    if (iter != m_registryHash.end())
    {
        Registry & registry = iter->second;
        for (MCUint i = 0; i < registry.size(); i++)
        {
            if (registry[i] == generator && iter->first == &object)
            {
                registry[i] = registry.back();
                registry.pop_back();
                break;
            }
        }

        if (!registry.size())
        {
            m_registryHash.erase(iter);
        }
    }
}

void MCForceRegistry::removeForceGenerators(MCObject & object)
{
    auto iter = m_registryHash.find(&object);
    if (iter != m_registryHash.end())
    {
        m_registryHash.erase(iter);
    }
}

void MCForceRegistry::clear()
{
    m_registryHash.clear();
}
