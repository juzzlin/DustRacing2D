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

#include "mcshapeview.hh"

MCTypeRegistry MCShapeView::m_typeRegistry;

MCShapeView::MCShapeView(const std::string & handle)
    : m_viewId(MCShapeView::m_typeRegistry.registerType(handle))
    , m_shaderProgram(MCGLScene::instance().defaultShaderProgram())
    , m_shadowShaderProgram(MCGLScene::instance().defaultShadowShaderProgram())
    , m_hasShadow(true)
    , m_batchMode(false)
{}

MCShapeView::~MCShapeView()
{
}

void MCShapeView::setShaderProgram(MCGLShaderProgramPtr shaderProgram)
{
    m_shaderProgram = shaderProgram;
}

void MCShapeView::setShadowShaderProgram(MCGLShaderProgramPtr shaderProgram)
{
    m_shadowShaderProgram = shaderProgram;
}

MCGLShaderProgramPtr MCShapeView::shaderProgram() const
{
    return m_shaderProgram;
}

MCGLShaderProgramPtr MCShapeView::shadowShaderProgram() const
{
    return m_shadowShaderProgram;
}

void MCShapeView::setHasShadow(bool flag)
{
    m_hasShadow = flag;
}

bool MCShapeView::hasShadow() const
{
    return m_hasShadow;
}

void MCShapeView::render(const MCVector3d<MCFloat> &, MCFloat, MCCamera *)
{
}

void MCShapeView::renderShadow(const MCVector3d<MCFloat> &, MCFloat, MCCamera *)
{
}

void MCShapeView::setBatchMode(bool enable)
{
    m_batchMode = enable;
}

bool MCShapeView::batchMode() const
{
    return m_batchMode;
}

void MCShapeView::beginBatch()
{
}

void MCShapeView::endBatch()
{
}

void MCShapeView::beginShadowBatch()
{
}

void MCShapeView::endShadowBatch()
{
}

MCUint MCShapeView::viewId() const
{
    return m_viewId;
}

void MCShapeView::setHandle(std::string handle)
{
    m_viewId = MCShapeView::m_typeRegistry.registerType(handle);
}

void MCShapeView::setColor(const MCGLColor & color)
{
    m_color = color;
}

const MCGLColor & MCShapeView::color() const
{
    return m_color;
}
