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

#include "mcsurfaceview.hh"
#include "mccamera.hh"
#include "mcglshaderprogram.hh"
#include "mcsurface.hh"

class MCSurfaceView::Impl
{
public:
    Impl(MCSurfaceView & self, MCSurfacePtr surface)
      : m_self(self)
      , m_surface(surface)
    {
        if (surface)
        {
            m_surface->setShaderProgram(m_self.shaderProgram());
            m_surface->setShadowShaderProgram(m_self.shadowShaderProgram());

            updateBBox();
        }
    }

    ~Impl()
    {
    }

    void setSurface(MCSurfacePtr surface)
    {
        m_surface = surface;
        m_surface->setShaderProgram(m_self.shaderProgram());
        m_surface->setShadowShaderProgram(m_self.shadowShaderProgram());

        m_self.setHandle(surface->handle());

        updateBBox();
    }

    MCSurfacePtr surface() const
    {
        return m_surface;
    }

    void setShaderProgram(MCGLShaderProgramPtr program)
    {
        m_surface->setShaderProgram(program);
    }

    void setShadowShaderProgram(MCGLShaderProgramPtr program)
    {
        m_surface->setShadowShaderProgram(program);
    }

    void render(const MCVector3dF & l, float angle, MCCamera * p)
    {
        m_surface->setScale(m_self.scale());
        m_surface->render(p, l, angle);
    }

    void renderShadow(const MCVector3dF & l, float angle, MCCamera * p)
    {
        m_surface->setScale(m_self.scale());
        m_surface->renderShadow(p, l, angle);
    }

    void bind()
    {
        m_surface->bind();
    }

    void bindShadow()
    {
        m_surface->bindShadow();
    }

    void release()
    {
        m_surface->release();
    }

    void releaseShadow()
    {
        m_surface->releaseShadow();
    }

    const MCBBoxF & bbox() const
    {
        return m_bbox;
    }

    void setColor(const MCGLColor & color)
    {
        m_surface->setColor(color);
    }

    void updateBBox()
    {
        // TODO: Fix this! The view should know the angle of the
        // shape somehow. Now we just return a naive bbox.

        const float w = m_surface->width() / 2;
        const float h = m_surface->height() / 2;
        const float r = std::max(w, h);

        m_bbox = MCBBoxF(-r * m_self.scale().i(), -r * m_self.scale().j(), r * m_self.scale().i(), r * m_self.scale().j());
    }

    MCGLObjectBase * object() const
    {
        return m_surface.get();
    }

private:
    MCSurfaceView & m_self;

    MCSurfacePtr m_surface;

    MCBBoxF m_bbox;
};

MCSurfaceView::MCSurfaceView(const std::string & viewId, MCSurfacePtr surface)
  : MCShapeView(viewId)
  , m_impl(std::make_unique<Impl>(*this, surface))
{
}

MCSurfaceView::~MCSurfaceView() = default;

void MCSurfaceView::setSurface(MCSurfacePtr surface)
{
    m_impl->setSurface(surface);
}

MCSurfacePtr MCSurfaceView::surface() const
{
    return m_impl->surface();
}

void MCSurfaceView::setShaderProgram(MCGLShaderProgramPtr program)
{
    MCShapeView::setShaderProgram(program);
    m_impl->setShaderProgram(program);
}

void MCSurfaceView::setShadowShaderProgram(MCGLShaderProgramPtr program)
{
    MCShapeView::setShadowShaderProgram(program);
    m_impl->setShadowShaderProgram(program);
}

void MCSurfaceView::render(const MCVector3dF & l, float angle, MCCamera * p)
{
    m_impl->render(l, angle, p);
}

void MCSurfaceView::renderShadow(const MCVector3dF & l, float angle, MCCamera * p)
{
    m_impl->renderShadow(l, angle, p);
}

void MCSurfaceView::bind()
{
    m_impl->bind();
}

void MCSurfaceView::bindShadow()
{
    m_impl->bindShadow();
}

void MCSurfaceView::release()
{
    m_impl->release();
}

void MCSurfaceView::releaseShadow()
{
    m_impl->releaseShadow();
}

const MCBBoxF & MCSurfaceView::bbox() const
{
    return m_impl->bbox();
}

void MCSurfaceView::setColor(const MCGLColor & color)
{
    MCShapeView::setColor(color);
    m_impl->setColor(color);
}

void MCSurfaceView::setScale(const MCVector3dF & scale)
{
    MCShapeView::setScale(scale);
    m_impl->updateBBox();
}

MCGLObjectBase * MCSurfaceView::object() const
{
    return m_impl->object();
}
