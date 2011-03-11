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

#include <QFile>
#include <QTextStream>

#include "trackio.h"
#include "trackdata.h"
#include "tracktile.h"

TrackIO::TrackIO()
{}

bool TrackIO::save(const TrackData * trackData, QString path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);

        out << QString("<track version=\"%1\" name=\"%2\" cols=\"%3\" rows=\"%4\">\n")
                .arg(EDITOR_VERSION)
                .arg(trackData->name())
                .arg(trackData->cols())
                .arg(trackData->rows());

        for (unsigned int i = 0; i < trackData->cols(); i++)
        {
            for (unsigned int j = 0; j < trackData->rows(); j++)
            {
                if (TrackTile * tile = trackData->tile(i, j))
                {
                    out << QString("<tile id=\"%1\" i=\"%2\" j=\"%3\" o=\"%4\"/>\n")
                            .arg(tile->tileType())
                            .arg(i)
                            .arg(j)
                            .arg(tile->rotation());
                }
            }
        }

        out << QString("</track>");

        file.close();
    }

    return true;
}

bool TrackIO::open(TrackData * trackData, QString path)
{
    QFile in(path);
    return true;
}

