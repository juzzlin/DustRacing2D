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

#include "../mcobject.hh"
#include "mcforceregistry.hh"
#include "mcforceregistryimpl.hh"
#include "mcforcegenerator.hh"

MCForceRegistryImpl::MCForceRegistryImpl() :
    m_registryHash(),
    m_owned()
{}

MCForceRegistryImpl::~MCForceRegistryImpl()
{
    auto iter(m_owned.begin());
    while (iter != m_owned.end()) {
        delete *iter;
        iter++;
    }
    m_owned.clear();
}

void MCForceRegistryImpl::update()
{
    auto iter = m_registryHash.begin();
    while (iter != m_registryHash.end())
    {
        Registry & registry = iter->second;
        for (MCUint i = 0; i < registry.size(); i++) {
            if (iter->first->index() != -1) {
                if (registry[i]->enabled()) {
                    registry[i]->updateForce(*iter->first);
                }
            }
        }

        iter++;
    }
}

void MCForceRegistryImpl::remove(MCForceGenerator & generator,
    MCObject & object)
{
    auto iter = m_registryHash.find(&object);
    if (iter != m_registryHash.end())
    {
        Registry & registry = iter->second;
        for (MCUint i = 0; i < registry.size(); i++) {
            if (registry[i] == &generator &&
                iter->first == &object) {
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

MCForceRegistry::MCForceRegistry() :
    m_pImpl(new MCForceRegistryImpl)
{}

void MCForceRegistry::addForceGenerator(MCForceGenerator & generator,
    MCObject & object, bool takeOwnership)
{
    MCForceRegistryImpl::Registry & registry =
        m_pImpl->m_registryHash[&object];

    if (find(registry.begin(), registry.end(),
        &generator) == registry.end())
    {
        registry.push_back(&generator);

        if (takeOwnership) {
            m_pImpl->m_owned.insert(&generator);
        }
    }
}

void MCForceRegistry::removeForceGenerator(MCForceGenerator & generator,
    MCObject & object)
{
    m_pImpl->remove(generator, object);
}

void MCForceRegistry::update()
{
    m_pImpl->update();
}

void MCForceRegistry::clear()
{
    m_pImpl->m_registryHash.clear();
}

