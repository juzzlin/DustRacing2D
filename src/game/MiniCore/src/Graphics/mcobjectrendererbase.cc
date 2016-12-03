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

#include "mcobjectrendererbase.hh"

MCObjectRendererBase::MCObjectRendererBase(int maxBatchSize)
    : m_batchSize(0)
    , m_maxBatchSize(maxBatchSize)
    , m_hasShadow(false)
    , m_useAlphaBlend(false)
    , m_src(0)
    , m_dst(0)
{
}

void MCObjectRendererBase::setAlphaBlend(bool useAlphaBlend, GLenum src, GLenum dst)
{
    m_useAlphaBlend = useAlphaBlend;
    m_src           = src;
    m_dst           = dst;
}

int MCObjectRendererBase::maxBatchSize() const
{
    return m_maxBatchSize;
}

int MCObjectRendererBase::batchSize() const
{
    return m_batchSize;
}

void MCObjectRendererBase::setBatchSize(int batchSize)
{
    m_batchSize = batchSize;
}

void MCObjectRendererBase::setHasShadow(bool hasShadow)
{
    m_hasShadow = hasShadow;
}

bool MCObjectRendererBase::hasShadow() const
{
    return m_hasShadow;
}

bool MCObjectRendererBase::useAlphaBlend() const
{
    return m_useAlphaBlend;
}

GLenum MCObjectRendererBase::alphaSrc() const
{
    return m_src;
}

GLenum MCObjectRendererBase::alphaDst() const
{
    return m_dst;
}

MCObjectRendererBase::~MCObjectRendererBase()
{
}
