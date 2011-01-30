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

#ifndef TRACKIO_H
#define TRACKIO_H

class TrackData;

class TrackIO
{
public:

    //! Save given track data. Returns false if failed.
    static bool save(const TrackData * trackData, QString path);

    //! Load given track data. Returns false if failed.
    static bool open(TrackData * trackData, QString path);

private:

    TrackIO();
};

#endif // TRACKIO_H
