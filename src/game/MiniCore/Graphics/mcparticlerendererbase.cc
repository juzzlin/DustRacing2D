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

#include "mcparticlerendererbase.hh"

MCParticleRendererBase::MCParticleRendererBase(int maxBatchSize)
    : m_batchSize(0)
    , m_maxBatchSize(maxBatchSize)
    , m_hasShadow(false)
    , m_useAlphaBlend(false)
    , m_src(0)
    , m_dst(0)
{
}

void MCParticleRendererBase::setAlphaBlend(bool useAlphaBlend, GLenum src, GLenum dst)
{
    m_useAlphaBlend = useAlphaBlend;
    m_src           = src;
    m_dst           = dst;
}

int MCParticleRendererBase::maxBatchSize() const
{
    return m_maxBatchSize;
}

int MCParticleRendererBase::batchSize() const
{
    return m_batchSize;
}

void MCParticleRendererBase::setBatchSize(int batchSize)
{
    m_batchSize = batchSize;
}

void MCParticleRendererBase::setHasShadow(bool hasShadow)
{
    m_hasShadow = hasShadow;
}

bool MCParticleRendererBase::hasShadow() const
{
    return m_hasShadow;
}

bool MCParticleRendererBase::useAlphaBlend() const
{
    return m_useAlphaBlend;
}

GLenum MCParticleRendererBase::alphaSrc() const
{
    return m_src;
}

GLenum MCParticleRendererBase::alphaDst() const
{
    return m_dst;
}

MCParticleRendererBase::~MCParticleRendererBase()
{
}
