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

#ifndef MCRECTSHAPE_HH
#define MCRECTSHAPE_HH

#include "mcshape.hh"
#include "mcobbox.hh"
#include "mcedge.hh"

#include <tr1/memory>

using std::tr1::shared_ptr;

class MCRectShapeImpl;
class MCShapeView;
class MCObject;

/*! \class MCRectShape
 *  \brief Rectangular shape model
 *
 * Rectangular (oriented bbox) shape model for an MCObject.
 */
class MCRectShape : public MCShape
{
public:

  /*! Constructor
   * \param pParent Parent object
   * \param pView View for the shape. May be nullptr.
   */
  MCRectShape(MCObject * pParent, MCShapeView * pView, MCFloat width, MCFloat height);

  //! Destructor
  virtual ~MCRectShape();

  //! \reimp
  virtual void translate(const MCVector3d<MCFloat> & p);

  //! \reimp
  virtual void rotate(UINT a);

  //! \reimp
  virtual MCBBox<MCFloat> bbox() const;

  //! \reimp
  virtual bool contains(const MCVector2d<MCFloat> & p) const;

  //! \reimp
  virtual int interpenetrationDepth(const MCVector2d<MCFloat> & p1, const MCVector2d<MCFloat> & p2) const;

  //! \reimp
  virtual MCVector2d<MCFloat> contactNormal(const MCVector2d<MCFloat> & p) const;

  //! \brief Resize
  void resize(MCFloat width, MCFloat height);

  //! Return the oriented bbox to access vertices etc.
  const MCOBBox<MCFloat> & obbox() const;

  //! Return "nearest" edge for a point
  MCEdge<MCFloat> edgeForPoint(const MCVector2d<MCFloat> & p) const;

  //! Return the typeID
  static UINT typeID();

  //! \reimp
  virtual UINT instanceTypeID() const;

private:

  DISABLE_COPY(MCRectShape);
  DISABLE_ASSI(MCRectShape);
  shared_ptr<MCRectShapeImpl> const m_pImpl;
};

#endif // MCRECTSHAPE_HH
