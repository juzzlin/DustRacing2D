// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QCloseEvent>
#include <QMainWindow>
#include <QString>

#include <memory>

class AboutDlg;
class ObjectModelLoader;
class QAction;
class QCheckBox;
class QSlider;
class QTextEdit;
class QToolBar;

class Mediator;
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
    ~MainWindow() override;

    //! Return the singleton MainWindow.
    static MainWindow * instance();

    //! Return the current tool bar action, or nullptr if none.
    QAction * currentToolBarAction() const;

    //! Print text to the console.
    void console(QString text);

    //! Get reference to the object loaded.
    ObjectModelLoader & objectModelLoader() const;

    //! Set Windows title + the given file name.
    void setTitle(QString openFileName);

    //! Return true if objects will be rotated at random.
    bool randomlyRotateObjects() const;

public slots:

    //! Begin marking the route.
    void beginSetRoute();

    void enableUndo(bool enable);

    //! End marking the route.
    void endSetRoute();

    void setVisible(bool visible) override;

    void updateScaleSlider(int value);

protected:
    void closeEvent(QCloseEvent * event) override;

private slots:

    void clearRoute();

    bool doOpenTrack(QString fileName);

    void handleToolBarActionClick(QAction * action);

    void initializeNewTrack();

    void saveTrack();

    void saveAsTrack();

    void setTrackProperties();

    void setupTrackAfterUndoOrRedo();

    void showAboutDlg();

    void showAboutQtDlg();

    void showTip();

    void openArgTrack();

    void openTrack();

    void updateScale(int value);

private:
    void addObjectsToToolBar();

    void clearEditMode();

    void createWidgets();

    void fitScale();

    void init();

    bool loadObjectModels(QString objectFilePath);

    void populateMenuBar();

    void populateToolBar();

    void setActionStatesOnNewTrack();

    std::unique_ptr<ObjectModelLoader> m_objectModelLoader;

    std::unique_ptr<Mediator> m_mediator;

    AboutDlg * m_aboutDlg;

    QTextEdit * m_console;

    QAction * m_saveAction = nullptr;

    QAction * m_saveAsAction = nullptr;

    QAction * m_currentToolBarAction = nullptr;

    QAction * m_undoAction = nullptr;

    QAction * m_redoAction = nullptr;

    QAction * m_clearRouteAction = nullptr;

    QAction * m_setRouteAction = nullptr;

    QAction * m_setTrackPropertiesAction = nullptr;

    QSlider * m_scaleSlider = nullptr;

    QToolBar * m_toolBar = nullptr;

    QCheckBox * m_randomRotationCheck = nullptr;

    QString m_argTrackFile;

    bool m_saved = false;

    const char * m_settingsGroup = "MainWindow";

    const unsigned int m_minZoom = 0;

    const unsigned int m_maxZoom = 200;

    const unsigned int m_initZoom = 100;

    const int m_consoleHeight = 64;

    static MainWindow * m_instance;
};

#endif // MAINWINDOW_HPP
