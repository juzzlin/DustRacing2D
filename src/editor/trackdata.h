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

class TrackTile;

class TrackData
{
public:

  //! Constructor
  TrackData(QString name, unsigned int cols, unsigned rows);

  //! Destructor
  virtual ~TrackData();

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

  //! Set finish line to lie between given tile coordinates.
  //! Returns false on impossible coordinates or if coordinates out-of-range.
  bool setFinishLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);

  //! Get finish line.
  void finishLine(unsigned int & x0, unsigned int & y0, unsigned int & x1, unsigned int & y1) const;

  //! Returns true if finish line is set.
  bool finishLineSet() const;

private:

  QString m_name;
  QString m_fileName;
  unsigned int m_cols, m_rows;
  QVector<QVector<TrackTile *> > m_map;
  bool m_finishLineSet;
  unsigned int m_flx0, m_fly0, m_flx1, m_fly1;
};

#endif // TRACKDATA_H
