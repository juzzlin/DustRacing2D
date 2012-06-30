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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QCloseEvent>
#include <QString>

class AboutDlg;
class EditorData;
class EditorView;
class EditorScene;
class ObjectModelLoader;
class QAction;
class QSlider;
class QTextEdit;
class QToolBar;
class TrackData;

/*! \class MainWindow
 *  \brief The main window of the editor.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    //! Constructor.
    //! The constructor tries to load object models from
    //! Config::DATA_PATH/objects.conf.
    //! \param trackFile Track given as command line argument.
    explicit MainWindow(QString trackFile = "");
    
    //! Destructor
    ~MainWindow();

    //! Return the singleton MainWindow.
    static MainWindow * instance();

    //! Return the current tool bar action, or nullptr if none.
    QAction * currentToolBarAction() const;

    //! Print text to the console.
    void console(QString text);

    //! Get reference to the view.
    EditorView & editorView() const;

    //! Get reference to the scene.
    EditorScene & editorScene() const;

    //! Get reference to the data.
    EditorData & editorData() const;

    //! Get reference to the object loaded.
    ObjectModelLoader & objectModelLoader() const;

public slots:

    //! Begin marking the route.
    void beginSetRoute();

    //! End marking the route.
    void endSetRoute();

protected:

    //! \reimp
    void closeEvent(QCloseEvent * event);

private slots:

    void clear();
    void clearRoute();
    bool doOpenTrack(QString fileName);
    void enlargeHorSize();
    void enlargeVerSize();
    void handleToolBarActionClick(QAction * action);
    void initializeNewTrack();
    void setTrackProperties();
    void openTrack();
    void saveTrack();
    void saveAsTrack();
    void showAboutDlg();
    void updateScale(int value);

private:

    void addObjectsToToolBar();
    void init();
    bool loadObjectModels(QString objectFilePath);
    void populateMenuBar();
    void populateToolBar();
    void setActionStatesOnNewTrack();

    ObjectModelLoader * m_objectModelLoader;
    AboutDlg          * m_aboutDlg;
    EditorData        * m_editorData;
    EditorScene       * m_editorScene;
    EditorView        * m_editorView;
    QTextEdit         * m_console;
    QAction           * m_saveAction;
    QAction           * m_saveAsAction;
    QAction           * m_currentToolBarAction;
    QAction           * m_clearAllAction;
    QAction           * m_enlargeHorSize;
    QAction           * m_enlargeVerSize;
    QAction           * m_clearRouteAction;
    QAction           * m_setRouteAction;
    QAction           * m_setTrackPropertiesAction;
    QSlider           * m_scaleSlider;
    QToolBar          * m_toolBar;

    static MainWindow * m_instance;
};

#endif // MAINWINDOW_HPP

