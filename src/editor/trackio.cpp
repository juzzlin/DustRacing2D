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

#include <QFile>
#include <QTextStream>

#include "trackio.h"
#include "trackdata.h"

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

