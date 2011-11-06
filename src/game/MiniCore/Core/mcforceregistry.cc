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
#include "mcforceregistryimpl.hh"
#include "mcforcegenerator.hh"

MCForceRegistryImpl::MCForceRegistryImpl() :
    m_registry(),
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
    for (MCUint i = 0; i < m_registry.size(); i++) {
        if (m_registry[i].second->index() != -1) {
            m_registry[i].first->updateForce(m_registry[i].second);
        }
    }
}

void MCForceRegistryImpl::remove(MCForceGenerator * generator,
    MCObject * object)
{
    for (MCUint i = 0; i < m_registry.size(); i++) {
        if (m_registry[i].first == generator &&
                m_registry[i].second == object) {
            m_registry[i] = m_registry.back();
            m_registry.pop_back();
            return;
        }
    }
}

MCForceRegistry::MCForceRegistry() :
    m_pImpl(new MCForceRegistryImpl)
{}

void MCForceRegistry::addForceGenerator(MCForceGenerator * generator,
    MCObject * object, bool takeOwnership)
{
    m_pImpl->m_registry.push_back(
        MCForceRegistryImpl::ForceObjectPair(generator, object));

    if (takeOwnership) {
        m_pImpl->m_owned.insert(generator);
    }
}

void MCForceRegistry::removeForceGenerator(MCForceGenerator * generator,
    MCObject * object)
{
    m_pImpl->remove(generator, object);
}

void MCForceRegistry::update()
{
    m_pImpl->update();
}

void MCForceRegistry::clear()
{
    m_pImpl->m_registry.clear();
}

