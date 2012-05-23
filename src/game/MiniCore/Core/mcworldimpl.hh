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

#ifndef MCWORLDIMPL_HH
#define MCWORLDIMPL_HH

#include "mctypes.hh"
#include "Physics/mccollisiondetector.hh"
#include "Physics/mcimpulsegenerator.hh"
#include "Physics/mcforceregistry.hh"
#include <memory>
#include <unordered_set>
#include <vector>

class MCObject;
class MCWorld;
class MCObjectTree;
class MCContact;

//! Implementation class for MCWorld.
class MCWorldImpl
{
public:
    MCWorldImpl();
    virtual ~MCWorldImpl();
    void addObject(MCObject & object);
    void processRemovedObjects();
    void removeObject(MCObject & object);
private:
    void integrate(MCFloat step);
    void detectCollisions();
    void generateImpulses();
    void resolvePositions(MCFloat accuracy);
    void addToLayerMap(MCObject &);
    void removeFromLayerMap(MCObject &);
    MCContact * getDeepestInterpenetration(const std::vector<MCContact *> & contacts);
    void render(MCCamera * p);
    void renderShadows(MCCamera * p);
    static MCWorld * pInstance;
    MCForceRegistry forceRegistry;
    MCCollisionDetector collisionDetector;
    MCImpulseGenerator impulseGenerator;
    MCObjectTree * pObjectTree;
    MCFloat minX, maxX, minY, maxY, minZ, maxZ;
    static MCFloat metersPerPixel;
    static MCFloat metersPerPixelSquared;
    typedef std::unordered_set<MCObject *> LayerHash;
    LayerHash layers[MCWorld::MaxLayers];
    bool depthTestEnabled[MCWorld::MaxLayers];
    MCWorld::ObjectVector objs;
    MCWorld::ObjectVector removeObjs;
    friend class MCWorld;
};

#endif // MCWORLDIMPL_HH
