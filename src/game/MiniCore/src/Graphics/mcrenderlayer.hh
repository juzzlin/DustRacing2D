// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCRENDERLAYER_HH
#define MCRENDERLAYER_HH

#include <map>
#include <set>
#include <vector>

class MCCamera;
class MCObject;
class MCParticle;

class MCRenderLayer
{
public:

    MCRenderLayer();

    void clear();

    void setDepthTestEnabled(bool enable);

    bool depthTestEnabled() const;

    void setDepthMaskEnabled(bool enable);

    bool depthMaskEnabled() const;

    typedef int ObjectTypeId;
    typedef std::map<ObjectTypeId, std::vector<MCObject *> > BatchMap;
    typedef std::map<MCCamera *, BatchMap> CameraBatchMap;

    CameraBatchMap & objectBatches();

    CameraBatchMap & particleBatches();

private:

    bool m_depthTestEnabled;

    bool m_depthMaskEnabled;

    CameraBatchMap m_objectBatches;

    CameraBatchMap m_particleBatches;
};

#endif // MCRENDERLAYER_HH
