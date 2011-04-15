// This file is part of Dust Rallycross (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#ifndef TRACKDATA_H
#define TRACKDATA_H

#include <QString>
#include <QVector>

#include "route.h"

class TrackTile;

class TrackData
{
public:

  //! Constructor
  TrackData(QString name, unsigned int cols, unsigned rows);

  //! Get name
  QString name() const;

  //! Get file name
  QString fileName() const;

  //! Set file name
  void setFileName(QString fileName);

  //! Get column count
  unsigned int cols() const;

  //! Get row count
  unsigned int rows() const;

  //! Set given tile to given coordinates.
  //! Returns false if impossible coordinates.
  bool setTile(unsigned int x, unsigned int y, TrackTile * pTile);

  //! Get tile at given coordinates.
  //! Returns NULL if no tile set or impossible coordinates.
  TrackTile * tile(unsigned int x, unsigned int y) const;

  //! Get route object.
  Route & route();

private:

  QString m_name;
  QString m_fileName;
  unsigned int m_cols, m_rows;
  QVector<QVector<TrackTile *> > m_map;
  unsigned int m_flx0, m_fly0, m_flx1, m_fly1;
  Route m_route;
};

#endif // TRACKDATA_H
