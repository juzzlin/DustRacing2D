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

#include <memory>
#include <unordered_set>
#include <vector>

class MCObject;
class MCWorld;
class MCQuadtree;
class MCForceRegistry;
class MCContactResolver;
class MCContact;

class MCWorldImpl
{
public:
  MCWorldImpl();
  virtual ~MCWorldImpl();
  void removeObject(MCObject * p);
  void addObject(MCObject * p);
  void removeObjects();

private:
  void integrate(MCFloat step);
  void detectCollisions();
  void processContacts();
  void processContacts(MCObject * p);
  void processContact(MCObject * pObject, MCContact * pContact);
  void addToLayerMap(MCObject *);
  void removeFromLayerMap(MCObject *);
  MCContact * getDeepestInterpenetration(
      const std::vector<MCContact *> & contacts);
  void render(MCCamera * p);
  void renderShadows(MCCamera * p);
  static MCWorld * m_pInstance;
  MCForceRegistry * m_pForceRegistry;
  MCContactResolver * m_pResolver;
  MCQuadtree * m_pQuadtree;
  MCFloat minX, maxX, minY, maxY, minZ, maxZ;
  MCVector3d<MCFloat> m_gravity;
  typedef std::unordered_set<MCObject *> LayerHash;
  LayerHash m_layers[MCWorld::MAX_LAYERS];
  MCWorld::ObjectVector m_objs;
  MCWorld::ObjectVector m_removeObjs;
  friend class MCWorld;
};

#endif // MCWORLDIMPL_HH
