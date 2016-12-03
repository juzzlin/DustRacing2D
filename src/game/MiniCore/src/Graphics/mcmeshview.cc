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

#include "mcmeshview.hh"

#include "mccamera.hh"
#include "mcglshaderprogram.hh"
#include "mcmesh.hh"
#include "mcmeshview.hh"

MCMeshView::MCMeshView(const std::string & viewId, MCMesh * mesh)
    : MCShapeView(viewId)
    , m_mesh(mesh)
{
    if (m_mesh)
    {
        updateBBox();
    }
}

MCMeshView::~MCMeshView()
{}

void MCMeshView::updateBBox()
{
    // TODO: Fix this! The view should know the angle of the
    // shape somehow. Now we just return a naive bbox.

    const MCFloat w = m_mesh->width() / 2;
    const MCFloat h = m_mesh->height() / 2;
    const MCFloat r = std::max(w, h);

    m_bbox = MCBBoxF(-r, -r, r, r);
}

void MCMeshView::setMesh(MCMesh & mesh)
{
    m_mesh = &mesh;
    m_mesh->setShaderProgram(shaderProgram());
    m_mesh->setShadowShaderProgram(shadowShaderProgram());

    updateBBox();
}

MCMesh * MCMeshView::mesh() const
{
    return m_mesh;
}

void MCMeshView::setShaderProgram(MCGLShaderProgramPtr program)
{
    if (m_mesh)
    {
        MCShapeView::setShaderProgram(program);
        m_mesh->setShaderProgram(program);
    }
}

void MCMeshView::setShadowShaderProgram(MCGLShaderProgramPtr program)
{
    if (m_mesh)
    {
        MCShapeView::setShadowShaderProgram(program);
        m_mesh->setShadowShaderProgram(program);
    }
}

void MCMeshView::render(const MCVector3d<MCFloat> & l, MCFloat angle, MCCamera * p)
{
    if (m_mesh)
    {
        m_mesh->setScale(1.0, 1.0, 1.0);
        m_mesh->render(p, l, angle, !batchMode());
    }
}

void MCMeshView::renderShadow(const MCVector3d<MCFloat> & l, MCFloat angle, MCCamera * p)
{
    if (m_mesh)
    {
        m_mesh->setScale(1.0, 1.0, 1.0);
        m_mesh->renderShadow(p, l, angle, !batchMode());
    }
}

void MCMeshView::beginBatch()
{
    if (batchMode() && m_mesh)
    {
        m_mesh->bind();
    }
}

void MCMeshView::beginShadowBatch()
{
    if (batchMode() && m_mesh)
    {
        m_mesh->bindShadow();
    }
}

const MCBBoxF & MCMeshView::bbox() const
{
    return m_bbox;
}
