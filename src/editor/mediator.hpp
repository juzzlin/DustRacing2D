// This file is part of Dust Racing 2D.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include <QString>

#include "editormode.hpp"
#include "targetnode.hpp"

#include <memory>
#include <tuple>

class DragAndDropStore;
class EditorData;
class EditorView;
class MainWindow;
class TrackTile;
class Object;
class QAction;
class QGraphicsScene;
class QLayout;

/*! Acts as a communication channel between MainWindow and editor components:
 *
 *  - MainWindow <-> Mediator <-> QGraphicsScene / EditorView / EditorData
 *  - EditorView <-> Mediator <-> EditorData
 */
class Mediator
{
public:
    explicit Mediator(MainWindow & mainWindow);

    ~Mediator();

    void addCurrentToolBarObjectToScene(QPointF clickedScenePos);

    void addItem(QGraphicsItem * item);

    void addObject(std::shared_ptr<Object> object);

    void addViewToLayout(QLayout * layout);

    bool beginSetRoute();

    void clearEditMode();

    void clearRoute();

    int cols();

    void console(QString message);

    QString currentFileName() const;

    QAction * currentToolBarAction() const;

    DragAndDropStore & dadStore();

    void deleteColumn();

    void deleteRow();

    void endSetRoute();

    void enableUndo(bool enable);

    void floodFill(TrackTile & tile, QAction * action, const QString & typeToFill);

    int fitScale();

    void initScene();

    std::tuple<bool, QString, int, int> initializeNewTrack();

    void insertColumnAfter();

    void insertColumnBefore();

    void insertRowAfter();

    void insertRowBefore();

    bool isUndoable() const;

    bool isRedoable() const;

    void mouseWheelZoom(int delta);

    bool openTrack(QString fileName);

    void pushNewTargetNodeToRoute(QPointF pos);

    void redo();

    void removeItem(QGraphicsItem * item);

    void removeObject(Object & object);

    bool routeHasNodes() const;

    int rows();

    void saveUndoPoint();

    bool saveTrackData();

    bool saveTrackDataAs(QString fileName);

    void setScale(int value);

    void setMode(EditorMode newMode);
    EditorMode mode() const;

    void setSelectedObject(Object * object);
    Object * selectedObject() const;

    void setSelectedTargetNode(TargetNode * tnode);
    TargetNode * selectedTargetNode() const;

    bool setTrackProperties();

    void showStatusBarMessage(QString message);

    void setupTrackAfterUndoOrRedo();

    void undo();

    void updateCoordinates(QPointF mappedPos);

    void updateView();

private:
    std::unique_ptr<EditorData> m_editorData;

    QGraphicsScene * m_editorScene;

    EditorView * m_editorView;

    MainWindow & m_mainWindow;

    EditorMode m_mode = EditorMode::None;
};

#endif // MEDIATOR_HPP
