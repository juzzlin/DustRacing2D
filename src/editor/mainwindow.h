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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QString>

class EditorData;
class EditorView;
class EditorScene;
class TrackData;
class QTextEdit;
class QAction;
class QSlider;
class QToolBar;

/*! \class MainWindow
 *  \brief The main window of the editor.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    //! Constructor
    MainWindow();
    
    //! Destructor
    ~MainWindow();

    //! Return the singleton MainWindow.
    static MainWindow * instance();

    //! Return the current tool bar action, or NULL if none.
    QAction * currentToolBarAction() const;

    //! Print text to the console.
    void console(QString text);

public slots:

    //! Begin marking the route.
    void beginSetRoute();

    //! End marking the route.
    void endSetRoute();

protected:

    //! \reimp
    void closeEvent(QCloseEvent * event);

private slots:

    void initializeNewTrack();
    void openTrack();
    void saveTrack();
    void saveAsTrack();
    void updateScale(int value);
    void handleToolBarActionClick(QAction * action);
    void clear();
    void clearRoute();

private:

    void populateMenuBar();
    void populateToolBar();
    void addTilesToScene();
    void removeTilesFromScene();

    EditorData  * m_editorData;
    EditorView  * m_editorView;
    EditorScene * m_editorScene;
    QTextEdit   * m_console;
    QAction     * m_saveAction;
    QAction     * m_saveAsAction;
    QAction     * m_currentToolBarAction;
    QAction     * m_clearAllAction;
    QAction     * m_clearRouteAction;
    QAction     * m_setRouteAction;
    QSlider     * m_scaleSlider;
    QToolBar    * m_toolBar;

    static MainWindow * m_instance;
};

#endif // MAINWINDOW_H

