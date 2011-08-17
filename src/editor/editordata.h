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

#ifndef EDITORDATA_H
#define EDITORDATA_H

#include <tr1/memory>
#include <QString>

class TrackData;

class EditorData
{
public:

    enum EditorMode {EM_NONE = 0, EM_SETROUTE, EM_SETTILETYPE};

    EditorData();
    bool loadTrackData(QString fileName);
    bool saveTrackData();
    bool saveTrackDataAs(QString fileName);
    void setTrackData(TrackData * newTrackData);
    bool canRouteBeSet() const;
    void beginSetRoute();
    void endSetRoute();
    TrackData * trackData();
    EditorMode mode() const;
    void setMode(EditorMode newMode);

private:

    EditorData(const EditorData & e);
    EditorData & operator= (const EditorData & e);

    std::tr1::shared_ptr<TrackData> m_trackData;
    EditorMode m_mode;
};

#endif // EDITORDATA_H
