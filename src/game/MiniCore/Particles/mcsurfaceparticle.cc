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

#include "mcsurfaceparticle.hh"
#include "mcsurface.hh"
#include "mcsurfaceview.hh"

MCSurfaceParticle::MCSurfaceParticle(const std::string & viewId)
: MCParticle(viewId)
{
    setView(new MCSurfaceView(viewId, nullptr, true));
    view()->setHasShadow(false);
}

void MCSurfaceParticle::beginBatch()
{
    view()->beginBatch();
}

void MCSurfaceParticle::endBatch()
{
    view()->endBatch();
}

void MCSurfaceParticle::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    static_cast<MCSurfaceView *>(view())->surface()->setColor(r, g, b, a);
}

void MCSurfaceParticle::setSurface(MCSurface & surface)
{
    static_cast<MCSurfaceView *>(view())->setSurface(surface);
}

void MCSurfaceParticle::setShaderProgram(MCGLShaderProgram * program)
{
    static_cast<MCSurfaceView *>(view())->surface()->setShaderProgram(program);
}

MCSurfaceParticle::~MCSurfaceParticle()
{
}
