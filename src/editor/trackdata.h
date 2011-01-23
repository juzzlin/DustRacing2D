// This file belongs to the "Dust" car racing game
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

#ifndef TRACKDATA_H
#define TRACKDATA_H

#include <QString>
#include <QVector>

class TrackTile;

class TrackData
{
public:

  //! Constructor
  TrackData(QString name, unsigned int horSize, unsigned verSize);

  //! Destructor
  virtual ~TrackData();

  //! Get name
  QString name() const;

  //! Get horizontal size
  unsigned int horSize() const;

  //! Get vertical size
  unsigned int verSize() const;

private:

  QString m_name;

  unsigned int m_horSize, m_verSize;

  QVector<QVector<TrackTile *> > m_map;
};

#endif // TRACKDATA_H
