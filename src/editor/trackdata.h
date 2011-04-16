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

#include "route.h"
#include "map.h"

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

  //! Get map object.
  Map & map();

  //! Get route object.
  const Map & map() const;

  //! Get route object.
  Route & route();

  //! Get route object.
  const Route & route() const;

private:

  QString m_name;
  QString m_fileName;
  Map     m_map;
  Route   m_route;
};

#endif // TRACKDATA_H
