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

#ifndef MCCIRCLESHAPE_HH
#define MCCIRCLESHAPE_HH

#include "mcshape.hh"
#include <tr1/memory>

using std::tr1::shared_ptr;

class MCCircleShapeImpl;
class MCShapeView;

//! Circular shape model
class MCCircleShape : public MCShape
{
public:

  /*! Constructor
   * \param radius  Radius of the shape
   * \param pParent Parent object
   * \param pView   View for the shape. May be nullptr.
   */
  MCCircleShape(MCObject * pParent, MCShapeView * pView, MCFloat radius);

  //! Destructor
  virtual ~MCCircleShape();

  //! \reimp
  bool contains(const MCVector2d<MCFloat> & p) const;

  //! \reimp
  int interpenetrationDepth(const MCVector2d<MCFloat> & p1, const MCVector2d<MCFloat> & p2) const;

  //! \reimp
  MCVector2d<MCFloat> contactNormal(const MCVector2d<MCFloat> & p) const;

  //! Return type id
  static MCUint typeID();

  //! \reimp
  virtual MCUint instanceTypeID() const;

  //! \reimp
  virtual MCBBox<MCFloat> bbox() const;

  //! Set the radius
  void setRadius(MCFloat r);

  //! Return the radius
  MCFloat radius() const;

private:

  DISABLE_COPY(MCCircleShape);
  DISABLE_ASSI(MCCircleShape);
  shared_ptr<MCCircleShapeImpl> const m_pImpl;
};

#endif // MCCIRCLESHAPE_HH
