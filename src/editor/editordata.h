// This file is part of Dust Racing (DustRAC).
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
#include <QPointF>

class MainWindow;
class TrackData;
class TrackTile;

//! Editor data includes data and functionality related to the current
//! race track and editing session.
class EditorData
{
public:

    //! Editing modes.
    enum EditorMode {EM_NONE = 0,
        EM_SET_ROUTE, EM_SET_TILE_TYPE, EM_ADD_OBJECT};

    //! Constructor.
    //! \param mainWindow Pointer to the main window.
    EditorData(MainWindow * mainWindow);

    //! Load track from the given file.
    bool loadTrackData(QString fileName);

    //! Save track data.
    bool saveTrackData();

    //! Sava track data to the given file.
    bool saveTrackDataAs(QString fileName);

    //! Set track data as the given data.
    void setTrackData(TrackData * newTrackData);

    //! Check if it's possible to set route.
    bool canRouteBeSet() const;

    //! This should be called before user starts
    //! setting the route.
    void beginSetRoute();

    //! This should be called after user ends
    //! setting the route.
    void endSetRoute();

    //! Adds line objects to the scenes visualizing
    //! the current route.
    //! \param closeLoop The route will be considered a
    //! closed loop if true.
    void addRouteLinesToScene(bool closeLoop);

    //! Removes line objects from the scene.
    void removeRouteLinesFromScene();

    //! Returns current track data object.
    TrackData * trackData();

    //! Returns current editing mode.
    EditorMode mode() const;

    //! Sets the current editing mode.
    void setMode(EditorMode newMode);

    //! Returns the tile being drag'n'dropped
    //! or nullptr.
    TrackTile * dragAndDropSourceTile() const;

    //! Set tile that is being drag'n'dropped.
    void setDragAndDropSourceTile(TrackTile * tile);

    //! Get source pos of the drag'n'drop.
    QPointF dragAndDropSourcePos() const;

    //! Set source pos of the drag'n'drop.
    void setDragAndDropSourcePos(QPointF pos);

private:

    EditorData(const EditorData & e);
    EditorData & operator= (const EditorData & e);

    std::tr1::shared_ptr<TrackData> m_trackData;
    EditorMode m_mode;
    TrackTile * m_dragAndDropSourceTile;
    QPointF m_dragAndDropSourcePos;
    MainWindow * m_mainWindow;
};

#endif // EDITORDATA_H
