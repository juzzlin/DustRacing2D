// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either Config::Editor 3 of the License, or
// (at your option) any later Config::Editor.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "mainwindow.h"

#include "../common/config.h"
#include "objectdata.h"
#include "objectloader.h"
#include "trackio.h"
#include "tracktile.h"
#include "editordata.h"
#include "editorview.h"
#include "editorscene.h"
#include "newtrackdialog.h"

#include "../common/trackdata.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QSettings>
#include <QSlider>
#include <QSplitter>
#include <QTextEdit>
#include <QTimer>
#include <QTransform>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow * MainWindow::m_instance = nullptr;

namespace
{
    const char *       SETTINGS_GROUP = "MainWindow";
    const int          MARGIN         = 0;
    const unsigned int MIN_ZOOM       = 0;
    const unsigned int MAX_ZOOM       = 200;
    const unsigned int INI_ZOOM       = 100;
    const int          CONSOLE_HEIGHT = 64;
}

MainWindow::MainWindow(QString trackFile)
: m_editorData(new EditorData(this))
, m_editorView(new EditorView(m_editorData, this))
, m_editorScene(new EditorScene(this))
, m_console(new QTextEdit(this))
, m_saveAction(nullptr)
, m_saveAsAction(nullptr)
, m_currentToolBarAction(nullptr)
, m_clearAllAction(nullptr)
, m_setRouteAction(nullptr)
, m_scaleSlider(new QSlider(Qt::Horizontal, this))
, m_toolBar(new QToolBar(this))
, m_objectLoader(new ObjectLoader)
{
    if (!m_instance)
    {
        m_instance = this;
    }
    else
    {
        qFatal("MainWindow already instantiated!");
    }

    // Init widgets
    init();

    console("CWD: " + QDir::currentPath());

    // Load object models that can be used to build tracks.
    const QString objectFilePath = QString(Config::Common::DATA_PATH) +
        QDir::separator() + "objects.conf";
    loadObjectModels(objectFilePath);

    if (!trackFile.isEmpty())
    {
        // Print a welcome message
        console(tr("Loading '%1'..").arg(trackFile));
        doOpenTrack(trackFile);
    }
    else
    {
        // Print a welcome message
        console(tr("Choose 'File -> New' or 'File -> Open' to start.."));
    }
}

void MainWindow::init()
{
    setWindowTitle(QString(Config::Editor::EDITOR_NAME) + " " +
        Config::Editor::EDITOR_VERSION);

    QSettings settings(Config::Editor::QSETTINGS_COMPANY_NAME,
        Config::Editor::QSETTINGS_SOFTWARE_NAME);

    // Read dialog size data
    settings.beginGroup(SETTINGS_GROUP);
    resize(settings.value("size", QSize(640, 480)).toSize());
    settings.endGroup();

    // Try to center the window.
    QRect geometry(QApplication::desktop()->availableGeometry());
    move(geometry.width() / 2 - width() / 2,
        geometry.height() / 2 - height() / 2);

    // Populate menu bar with actions
    populateMenuBar();

    // Set scene to the view
    m_editorView->setScene(m_editorScene);
    m_editorView->setSizePolicy(QSizePolicy::Preferred,
        QSizePolicy::Expanding);
    m_editorView->setMouseTracking(true);

    // Create a splitter
    QSplitter * splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    // Create layouts for slider, view and toolbar
    QVBoxLayout * centralLayout = new QVBoxLayout;
    QHBoxLayout * viewToolBarLayout = new QHBoxLayout;
    m_toolBar->setOrientation(Qt::Vertical);
    viewToolBarLayout->addWidget(m_editorView);
    viewToolBarLayout->addWidget(m_toolBar);
    centralLayout->addLayout(viewToolBarLayout);

    // Populate toolbar with actions
    populateToolBar();

    // Add zoom slider to the layout
    m_scaleSlider->setRange(MIN_ZOOM, MAX_ZOOM);
    m_scaleSlider->setValue(INI_ZOOM);
    m_scaleSlider->setTracking(false);
    m_scaleSlider->setTickInterval(10);
    m_scaleSlider->setTickPosition(QSlider::TicksBelow);
    connect(m_scaleSlider, SIGNAL(valueChanged(int)), this, SLOT(updateScale(int)));
    QHBoxLayout * sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(new QLabel(tr("Scale:")));
    sliderLayout->addWidget(m_scaleSlider);
    centralLayout->addLayout(sliderLayout);

    // Add console to the splitter and splitter to the layout
    m_console->setReadOnly(true);
    m_console->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    m_console->resize(m_console->width(), 50);
    QWidget * dummy = new QWidget(this);
    splitter->addWidget(dummy);
    dummy->setLayout(centralLayout);
    dummy->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    splitter->addWidget(m_console);

    // Set contents margins so that they look nice
    splitter->setContentsMargins(centralLayout->contentsMargins().left(),
                                 0,
                                 centralLayout->contentsMargins().right(),
                                 centralLayout->contentsMargins().bottom());

    centralLayout->setContentsMargins(0, centralLayout->contentsMargins().top(),
                                      0, centralLayout->contentsMargins().bottom());

    // Set splitter as the central widget
    setCentralWidget(splitter);

    QList<int> sizes;
    sizes << height() - CONSOLE_HEIGHT << CONSOLE_HEIGHT;
    splitter->setSizes(sizes);
}

bool MainWindow::loadObjectModels(QString objectFilePath)
{
    if (m_objectLoader->load(objectFilePath))
    {
        addObjectsToToolBar();
        return true;
    }
    else
    {
        const QString msg = tr("ERROR!!: Cannot load objects from '") +
            objectFilePath + tr("'");
        console(msg);
        return false;
    }
}

void MainWindow::addObjectsToToolBar()
{
    // Loop through all object models loaded
    // by the object loader.
    ObjectLoader::ObjectDataVector objects =
        m_objectLoader->getObjectsByCategory("tile");
    for (const ObjectData model : objects)
    {
        // Create the action.
        QAction * p = new QAction(QIcon(model.pixmap), model.role, this);

        // Set model role as the data.
        p->setData(QVariant(model.role));

        // Add it to the toolbar.
        m_toolBar->addAction(p);
    }
}

MainWindow * MainWindow::instance()
{
    return MainWindow::m_instance;
}

EditorScene * MainWindow::editorScene() const
{
    return m_editorScene;
}

ObjectLoader * MainWindow::objectLoader() const
{
    return m_objectLoader;
}

QAction * MainWindow::currentToolBarAction() const
{
    return m_currentToolBarAction;
}

void MainWindow::updateScale(int value)
{
    qreal scale = static_cast<qreal>(value) / 100;

    QTransform transform;
    transform.scale(scale, scale);
    m_editorView->setTransform(transform);

    console(QString("Scale set to %1%").arg(value));
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    // Open settings file
    QSettings settings(Config::Editor::QSETTINGS_COMPANY_NAME,
        Config::Editor::QSETTINGS_SOFTWARE_NAME);

    // Save window size
    settings.beginGroup(SETTINGS_GROUP);
    settings.setValue("size", size());
    settings.endGroup();

    event->accept();
}

void MainWindow::populateMenuBar()
{
    // Create "file"-menu
    QMenu * fileMenu = menuBar()->addMenu(tr("&File"));

    // Add "new"-action
    QAction * newAct = new QAction(tr("&New..."), this);
    newAct->setShortcut(QKeySequence("Ctrl+N"));
    fileMenu->addAction(newAct);
    connect(newAct, SIGNAL(triggered()), this, SLOT(initializeNewTrack()));

    // Add "open"-action
    QAction * openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence("Ctrl+O"));
    fileMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(openTrack()));

    // Add "save"-action
    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcut(QKeySequence("Ctrl+S"));
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(saveTrack()));
    m_saveAction->setEnabled(false);

    // Add "save as"-action
    m_saveAsAction = new QAction(tr("&Save as..."), this);
    m_saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsTrack()));
    m_saveAsAction->setEnabled(false);

    // Add "quit"-action
    QAction * quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(QKeySequence("Ctrl+W"));
    fileMenu->addAction(quitAct);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Create "edit"-menu
    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));

    // Add "clear all"-action
    m_clearAllAction = new QAction(tr("&Clear all"), this);
    editMenu->addAction(m_clearAllAction);
    connect(m_clearAllAction, SIGNAL(triggered()), this, SLOT(clear()));
    m_clearAllAction->setEnabled(false);

    // Create "route"-menu
    QMenu * routeMenu = menuBar()->addMenu(tr("&Route"));

    // Add "clear"-action
    m_clearRouteAction = new QAction(tr("Clear &route"), this);
    routeMenu->addAction(m_clearRouteAction);
    connect(m_clearRouteAction, SIGNAL(triggered()), this, SLOT(clearRoute()));

    // Add "set order"-action
    m_setRouteAction = new QAction(tr("&Set route.."), this);
    routeMenu->addAction(m_setRouteAction);
    connect(m_setRouteAction, SIGNAL(triggered()), this, SLOT(beginSetRoute()));
    m_setRouteAction->setEnabled(false);

    // Create "help"-menu
    QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about"-action
    QAction * aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    //  connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
}

void MainWindow::populateToolBar()
{
    // Add "select"-action
    QAction * p = new QAction(QIcon(QPixmap(Config::Editor::SELECT_PATH)),
                              tr("Select"), this);
    p->setData(QVariant(QString("select")));
    m_toolBar->addAction(p);

    // Add "clear"-action
    p = new QAction(QIcon(QPixmap(Config::Editor::CLEAR_PATH)),
                    tr("Clear"), this);
    p->setData(QVariant(QString("clear")));
    m_toolBar->addAction(p);

    connect(m_toolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(handleToolBarActionClick(QAction*)));

    m_toolBar->setEnabled(false);
}

void MainWindow::handleToolBarActionClick(QAction * action)
{
    if (action != m_currentToolBarAction)
    {
        m_currentToolBarAction = action;

        // Select-action
        if (action->data() == "select")
        {
            QApplication::restoreOverrideCursor();
            m_editorData->setMode(EditorData::EM_NONE);
        }
        // The user wants to set a tile type or clear it.
        else
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(action->icon().pixmap(32, 32)));
            m_editorData->setMode(EditorData::EM_SETTILETYPE);
        }
    }
    else
    {
        QApplication::restoreOverrideCursor();
        m_editorData->setMode(EditorData::EM_NONE);
        m_currentToolBarAction = nullptr;
    }
}

void MainWindow::openTrack()
{
    // Load recent path
    QSettings settings(Config::Editor::QSETTINGS_COMPANY_NAME,
        Config::Editor::QSETTINGS_SOFTWARE_NAME);

    settings.beginGroup(SETTINGS_GROUP);
    QString path = settings.value("recentPath",
        QDesktopServices::storageLocation(QDesktopServices::HomeLocation)).toString();
    settings.endGroup();

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open a track"),
                                                    path,
                                                    tr("Track Files (*.trk)"));
    doOpenTrack(fileName);
}

bool MainWindow::doOpenTrack(QString fileName)
{
    if (!QFile::exists(fileName))
    {
        console(QString(tr("ERROR!!: %1 doesn't exist.").arg(fileName)));
        return false;
    }

    removeTilesFromScene();

    m_editorData->loadTrackData(fileName);
    if (m_editorData->trackData())
    {
        console(QString(tr("Track '%1' opened.").arg(fileName)));

        // Save recent path
        QSettings settings(Config::Editor::QSETTINGS_COMPANY_NAME,
            Config::Editor::QSETTINGS_SOFTWARE_NAME);

        settings.beginGroup(SETTINGS_GROUP);
        settings.setValue("recentPath", fileName);
        settings.endGroup();

        m_saveAction->setEnabled(true);
        m_saveAsAction->setEnabled(true);
        m_toolBar->setEnabled(true);
        m_clearAllAction->setEnabled(true);
        m_setRouteAction->setEnabled(true);

        delete m_editorScene;
        m_editorScene = new EditorScene;

        QRectF newSceneRect(-MARGIN, -MARGIN,
            2 * MARGIN + m_editorData->trackData()->map().cols() * TrackTile::TILE_W,
            2 * MARGIN + m_editorData->trackData()->map().rows() * TrackTile::TILE_H);

        m_editorScene->setSceneRect(newSceneRect);
        m_editorView->setScene(m_editorScene);
        m_editorView->setSceneRect(newSceneRect);
        m_editorView->ensureVisible(0, 0, 0, 0);

        addTilesToScene();
        addRouteLinesToScene(true);

        return true;
    }
    else
    {
        console(QString(tr("Failed to open track '")) + fileName + "'.");
        return false;
    }

    return false;
}

void MainWindow::saveTrack()
{
    if (m_editorData->saveTrackData())
    {
        console(QString(tr("Track '")) + m_editorData->trackData()->fileName() + tr("' saved."));
    }
    else
    {
        console(QString(tr("Failed to save track '")) + m_editorData->trackData()->fileName() + "'.");
    }
}

void MainWindow::saveAsTrack()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Open a track"),
        QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
        tr("Track Files (*.trk)"));

    if (!fileName.endsWith(".trk"))
        fileName += ".trk";

    if (m_editorData->saveTrackDataAs(fileName))
    {
        console(QString(tr("Track '")) + fileName + tr("' saved."));
        m_saveAction->setEnabled(true);
    }
    else
    {
        console(QString(tr("Failed to save track as '")) + fileName + "'.");
    }
}

void MainWindow::initializeNewTrack()
{
    // Show a dialog asking some questions about the track
    NewTrackDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        const unsigned int cols = dialog.cols();
        const unsigned int rows = dialog.rows();

        removeTilesFromScene();

        m_editorData->setTrackData(new TrackData(dialog.name(), cols, rows));

        delete m_editorScene;
        m_editorScene = new EditorScene;

        QRectF newSceneRect(-MARGIN, -MARGIN,
            2 * MARGIN + cols * TrackTile::TILE_W,
            2 * MARGIN + rows * TrackTile::TILE_H);

        m_editorScene->setSceneRect(newSceneRect);
        m_editorView->setScene(m_editorScene);
        m_editorView->setSceneRect(newSceneRect);
        m_editorView->ensureVisible(0, 0, 0, 0);

        addTilesToScene();

        m_saveAsAction->setEnabled(true);
        m_toolBar->setEnabled(true);
        m_clearAllAction->setEnabled(true);
        m_setRouteAction->setEnabled(true);

        console(QString(tr("A new track '%1' created. Columns: %2, Rows: %3."))
            .arg(m_editorData->trackData()->name())
            .arg(m_editorData->trackData()->map().cols())
            .arg(m_editorData->trackData()->map().rows()));
    }
}

// TODO: Move to EditorData
void MainWindow::addTilesToScene()
{
    const unsigned int cols = m_editorData->trackData()->map().cols();
    const unsigned int rows = m_editorData->trackData()->map().rows();

    for (unsigned int i = 0; i < cols; i++)
        for (unsigned int j = 0; j < rows; j++)
            if (TrackTile * tile = m_editorData->trackData()->map().getTile(i, j))
                m_editorScene->addItem(tile);

    if (m_editorData->trackData()->map().getTile(0, 0))
        m_editorData->trackData()->map().getTile(0, 0)->setActive(true);
}

void MainWindow::addRouteLinesToScene(bool closeLoop)
{
    // Re-use this method
    m_editorData->addRouteLinesToScene(closeLoop);
}

// TODO: Move to EditorData
void MainWindow::removeTilesFromScene()
{
    if (m_editorData->trackData())
    {
        TrackTile::setActiveTile(nullptr);

        const unsigned int cols = m_editorData->trackData()->map().cols();
        const unsigned int rows = m_editorData->trackData()->map().rows();

        for (unsigned int i = 0; i < cols; i++)
            for (unsigned int j = 0; j < rows; j++)
                if (TrackTile * tile = m_editorData->trackData()->map().getTile(i, j))
                {
                    m_editorScene->removeItem(tile);
                    delete tile;
                }
    }
}

// TODO: Move to EditorData
void MainWindow::clear()
{
    const unsigned int cols = m_editorData->trackData()->map().cols();
    const unsigned int rows = m_editorData->trackData()->map().rows();

    for (unsigned int i = 0; i < cols; i++)
        for (unsigned int j = 0; j < rows; j++)
            if (TrackTile * p = m_editorData->trackData()->map().getTile(i, j))
                p->setTileType("clear");

    m_console->append(QString(tr("Tiles cleared.")));

    clearRoute();
}

// TODO: Move to EditorData
void MainWindow::clearRoute()
{
    m_editorData->removeRouteLinesFromScene();
    m_editorData->trackData()->route().clear();
    m_console->append(QString(tr("Route cleared.")));
}

void MainWindow::beginSetRoute()
{
    if (m_editorData->canRouteBeSet())
    {
        console(tr("Set route: begin."));
        QMessageBox::information(this, tr("Set route"), tr("Click on the tiles one by one and make a closed loop.\n"
                                                           "Start from the finish line tile.\n"
                                                           "Click on the finish line tile again to finish."));
        m_editorData->beginSetRoute();
    }
    else
    {
        QMessageBox::critical(this, tr("Set route"), tr("Invalid track. Route cannot be set."));
        console(tr("Set route: not a valid track."));
    }
}

void MainWindow::endSetRoute()
{
    m_editorData->endSetRoute();
    console(tr("Set route: route finished."));
}

void MainWindow::console(QString text)
{
    QDateTime date = QDateTime::currentDateTime();
    m_console->append(QString("(") + date.toString("hh:mm:ss") + "): " + text);
}

MainWindow::~MainWindow()
{
    delete m_editorData;
    delete m_objectLoader;
}
