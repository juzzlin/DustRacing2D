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

MCMeshView::MCMeshView(const std::string & viewId, MCMeshPtr mesh)
  : MCShapeView(viewId)
  , m_mesh(mesh)
{
    if (m_mesh)
    {
        // Take the initial view scale from the mesh
        setScale(mesh->scale());

        updateBBox();
    }
}

MCMeshView::~MCMeshView()
{
}

void MCMeshView::updateBBox()
{
    // TODO: Fix this! The view should know the angle of the
    // shape somehow. Now we just return a naive bbox.

    const float w = m_mesh->width() / 2;
    const float h = m_mesh->height() / 2;
    const float r = std::max(w, h);

    m_bbox = MCBBoxF(-r * scale().i(), -r * scale().j(), r * scale().i(), r * scale().j());
}

void MCMeshView::setMesh(MCMeshPtr mesh)
{
    m_mesh = mesh;
    m_mesh->setShaderProgram(shaderProgram());
    m_mesh->setShadowShaderProgram(shadowShaderProgram());

    updateBBox();
}

MCMeshPtr MCMeshView::mesh() const
{
    return m_mesh;
}

void MCMeshView::setShaderProgram(MCGLShaderProgramPtr program)
{
    MCShapeView::setShaderProgram(program);
    m_mesh->setShaderProgram(program);
}

void MCMeshView::setShadowShaderProgram(MCGLShaderProgramPtr program)
{
    MCShapeView::setShadowShaderProgram(program);
    m_mesh->setShadowShaderProgram(program);
}

void MCMeshView::render(const MCVector3dF & location, float angle, MCCamera * camera)
{
    m_mesh->setScale(scale());
    m_mesh->render(camera, location, angle);
}

void MCMeshView::renderShadow(const MCVector3dF & location, float angle, MCCamera * camera)
{
    m_mesh->setScale(scale());
    m_mesh->renderShadow(camera, location, angle);
}

const MCBBoxF & MCMeshView::bbox() const
{
    return m_bbox;
}

void MCMeshView::bind()
{
    m_mesh->bind();
}

void MCMeshView::bindShadow()
{
    m_mesh->bindShadow();
}

void MCMeshView::release()
{
    m_mesh->release();
}

void MCMeshView::releaseShadow()
{
    m_mesh->releaseShadow();
}

void MCMeshView::setScale(const MCVector3dF & scale)
{
    MCShapeView::setScale(scale);
    updateBBox();
}

MCGLObjectBase * MCMeshView::object() const
{
    return m_mesh.get();
}
