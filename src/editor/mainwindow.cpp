// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either Config::Editor 3 of the License, or
// (at your option) any later Config::Editor.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "mainwindow.hpp"

#include "../common/config.hpp"
#include "aboutdlg.hpp"
#include "editordata.hpp"
#include "editorview.hpp"
#include "mediator.hpp"
#include "newtrackdialog.hpp"
#include "object.hpp"
#include "objectmodel.hpp"
#include "objectmodelloader.hpp"
#include "trackdata.hpp"
#include "trackio.hpp"
#include "trackpropertiesdialog.hpp"
#include "tracktile.hpp"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSlider>
#include <QSplitter>
#include <QStandardPaths>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>
#include <QTransform>
#include <QVBoxLayout>

#include <cassert>

MainWindow * MainWindow::m_instance = nullptr;

MainWindow::MainWindow(QString trackFile)
  : m_objectModelLoader(std::make_unique<ObjectModelLoader>())
  , m_mediator(std::make_unique<Mediator>(*this))
  , m_aboutDlg(new AboutDlg(this))
  , m_console(new QTextEdit(this))
  , m_scaleSlider(new QSlider(Qt::Horizontal, this))
  , m_toolBar(new QToolBar(this))
  , m_randomRotationCheck(new QCheckBox(tr("Randomly rotate objects"), this))
  , m_argTrackFile(trackFile)
{
    if (!m_instance)
    {
        m_instance = this;
    }
    else
    {
        qFatal("MainWindow already instantiated!");
    }

    setWindowIcon(QIcon(":/dustrac-editor.png"));

    init();

    console("CWD: " + QDir::currentPath());

    // Load object models that can be used to build tracks.
    const QString objectFilePath = QString(Config::General::dataPath) + QDir::separator() + QString(Config::Editor::MODEL_CONFIG_FILE_NAME);
    loadObjectModels(objectFilePath);

    if (!m_argTrackFile.isEmpty())
    {
        QTimer::singleShot(0, this, &MainWindow::openArgTrack);
    }
    else
    {
        console(tr("Choose 'File -> New' or 'File -> Open' to start."));
    }
}

void MainWindow::openArgTrack()
{
    console(tr("Loading '%1'..").arg(m_argTrackFile));
    doOpenTrack(m_argTrackFile);
}

void MainWindow::setVisible(bool visible)
{
    QWidget::setVisible(visible);

    if (visible)
    {
        showTip();
    }
}

void MainWindow::updateScaleSlider(int value)
{
    m_scaleSlider->setValue(value);
}

void MainWindow::showTip()
{
    const auto commonTitle = tr("Save your work to correct location.");
#ifdef __unix__
    QMessageBox::information(
      this,
      commonTitle,
      QString(tr("To make the game find your circuits, save them to %1%2%3%2") + tr(", %4%2"))
        .arg(QDir::homePath())
        .arg(QDir::separator())
        .arg(Config::General::TRACK_SEARCH_PATH)
        .arg(Config::General::Unix::getXdgTrackSearchPath()));
#else
    QMessageBox::information(
      this,
      commonTitle,
      QString(tr("To make the game find your circuits, save them to %1%2%3%2"))
        .arg(QDir::homePath())
        .arg(QDir::separator())
        .arg(Config::General::TRACK_SEARCH_PATH));
#endif
}

void MainWindow::init()
{
    setTitle(tr("New file"));

    QSettings settings;

    // Read dialog size data
    settings.beginGroup(m_settingsGroup);
    resize(settings.value("size", QSize(640, 480)).toSize());
    settings.endGroup();

    // Try to center the window.
    QRect geometry(QApplication::desktop()->availableGeometry());
    move(geometry.width() / 2 - width() / 2,
         geometry.height() / 2 - height() / 2);

    m_mediator->initScene();

    populateMenuBar();

    createWidgets();
}

void MainWindow::createWidgets()
{
    // Create a splitter
    QSplitter * splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    // Create layouts for slider, view and toolbar
    QVBoxLayout * centralLayout = new QVBoxLayout;
    QHBoxLayout * viewToolBarLayout = new QHBoxLayout;
    m_toolBar->setOrientation(Qt::Vertical);
    m_mediator->addViewToLayout(viewToolBarLayout);
    viewToolBarLayout->addWidget(m_toolBar);
    centralLayout->addLayout(viewToolBarLayout);

    // Populate toolbar with actions
    populateToolBar();

    // Add zoom slider to the layout
    m_scaleSlider->setRange(m_minZoom, m_maxZoom);
    m_scaleSlider->setValue(m_initZoom);
    m_scaleSlider->setTracking(false);
    m_scaleSlider->setTickInterval(10);
    m_scaleSlider->setTickPosition(QSlider::TicksBelow);
    connect(m_scaleSlider, &QSlider::valueChanged, this, &MainWindow::updateScale);
    QHBoxLayout * sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(new QLabel(tr("Scale:")));
    sliderLayout->addWidget(m_scaleSlider);
    sliderLayout->addWidget(m_randomRotationCheck);
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
    sizes << height() - m_consoleHeight << m_consoleHeight;
    splitter->setSizes(sizes);
}

void MainWindow::setTitle(QString openFileName)
{
    setWindowTitle(
      QString(Config::Editor::EDITOR_NAME) + " " + Config::Editor::EDITOR_VERSION + " - " + openFileName);
}

bool MainWindow::randomlyRotateObjects() const
{
    return m_randomRotationCheck->isChecked();
}

bool MainWindow::loadObjectModels(QString objectFilePath)
{
    if (m_objectModelLoader->load(objectFilePath))
    {
        addObjectsToToolBar();
        return true;
    }
    else
    {
        const QString msg = tr("ERROR!!: Cannot load objects from '") + objectFilePath + tr("'");
        console(msg);
        return false;
    }
}

void MainWindow::addObjectsToToolBar()
{
    // Loop through all object models loaded
    // by the object loader.
    QVector<QString> categories;
    categories << "tile"
               << "free";
    for (QString category : categories)
    {
        ObjectModelLoader::ObjectDataVector objects =
          m_objectModelLoader->getObjectModelsByCategory(category);
        for (const ObjectModel & model : objects)
        {
            // Create the action.
            auto p = new QAction(QIcon(model.pixmap), model.role, this);

            // Set model role as the data.
            p->setData(QVariant(model.role));

            // Add it to the toolbar.
            m_toolBar->addAction(p);
        }
    }
}

MainWindow * MainWindow::instance()
{
    return MainWindow::m_instance;
}

ObjectModelLoader & MainWindow::objectModelLoader() const
{
    return *m_objectModelLoader;
}

QAction * MainWindow::currentToolBarAction() const
{
    return m_currentToolBarAction;
}

void MainWindow::updateScale(int value)
{
    m_mediator->setScale(value);

    console(QString(tr("Scale set to %1%")).arg(value));
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    // Open settings file
    QSettings settings;

    // Save window size
    settings.beginGroup(m_settingsGroup);
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
    connect(newAct, &QAction::triggered, this, &MainWindow::initializeNewTrack);

    // Add "open"-action
    QAction * openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence("Ctrl+O"));
    fileMenu->addAction(openAct);
    connect(openAct, &QAction::triggered, this, &MainWindow::openTrack);

    // Add "save"-action
    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAction->setShortcut(QKeySequence("Ctrl+S"));
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveTrack);
    m_saveAction->setEnabled(false);

    // Add "save as"-action
    m_saveAsAction = new QAction(tr("&Save as..."), this);
    m_saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveAsTrack);
    m_saveAsAction->setEnabled(false);

    // Add "quit"-action
    QAction * quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(QKeySequence("Ctrl+W"));
    fileMenu->addAction(quitAct);
    connect(quitAct, &QAction::triggered, this, &MainWindow::close);

    // Create "edit"-menu
    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));

    // Add "undo"-action
    m_undoAction = new QAction(tr("Undo"), this);
    m_undoAction->setShortcut(QKeySequence("Ctrl+Z"));
    editMenu->addAction(m_undoAction);
    connect(m_undoAction, &QAction::triggered, [this]() {
        m_mediator->undo();

        setupTrackAfterUndoOrRedo();

        m_undoAction->setEnabled(m_mediator->isUndoable());
        m_redoAction->setEnabled(m_mediator->isRedoable());
    });
    m_undoAction->setEnabled(false);

    // Add "redo"-action
    m_redoAction = new QAction(tr("Redo"), this);
    m_redoAction->setShortcut(QKeySequence("Ctrl+Shift+Z"));
    editMenu->addAction(m_redoAction);
    connect(m_redoAction, &QAction::triggered, [this]() {
        m_mediator->redo();

        setupTrackAfterUndoOrRedo();

        m_undoAction->setEnabled(m_mediator->isUndoable());
        m_redoAction->setEnabled(m_mediator->isRedoable());
    });
    m_redoAction->setEnabled(false);

    // Add "Set track properties"-action
    m_setTrackPropertiesAction = new QAction(tr("&Set properties.."), this);
    editMenu->addAction(m_setTrackPropertiesAction);
    connect(m_setTrackPropertiesAction, &QAction::triggered, this, &MainWindow::setTrackProperties);
    m_setTrackPropertiesAction->setEnabled(false);

    // Create "route"-menu
    QMenu * routeMenu = menuBar()->addMenu(tr("&Route"));

    // Add "clear"-action
    m_clearRouteAction = new QAction(tr("Clear &route"), this);
    routeMenu->addAction(m_clearRouteAction);
    connect(m_clearRouteAction, &QAction::triggered, this, &MainWindow::clearRoute);
    m_clearRouteAction->setEnabled(false);

    // Add "set order"-action
    m_setRouteAction = new QAction(tr("&Set route.."), this);
    routeMenu->addAction(m_setRouteAction);
    connect(m_setRouteAction, &QAction::triggered, this, &MainWindow::beginSetRoute);
    m_setRouteAction->setEnabled(false);

    // Create "help"-menu
    QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about"-action
    QAction * aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::showAboutDlg);

    // Add "about Qt"-action
    QAction * aboutQtAct = new QAction(tr("About &Qt"), this);
    helpMenu->addAction(aboutQtAct);
    connect(aboutQtAct, &QAction::triggered, this, &MainWindow::showAboutQtDlg);
}

void MainWindow::populateToolBar()
{
    // Add "select"-action
    QAction * p = new QAction(QIcon(QPixmap(Config::Editor::SELECT_ICON_PATH)), tr("Select"), this);
    p->setData(QVariant(QString("select")));
    m_toolBar->addAction(p);

    // Add "erase"-action
    p = new QAction(
      QIcon(QPixmap(Config::Editor::ERASE_ICON_PATH)), tr("Erase object"), this);
    p->setData(QVariant(QString("erase")));
    m_toolBar->addAction(p);

    // Add "clear"-action
    p = new QAction(QIcon(QPixmap(Config::Editor::CLEAR_ICON_PATH)), tr("Clear"), this);
    p->setData(QVariant(QString("clear")));
    m_toolBar->addAction(p);

    connect(m_toolBar, &QToolBar::actionTriggered, this, &MainWindow::handleToolBarActionClick);

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
            clearEditMode();
        }
        // The user wants to erase an object.
        else if (action->data() == "erase")
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(action->icon().pixmap(32, 32)));
            m_mediator->setMode(EditorMode::EraseObject);
        }
        // The user wants to clear a tile.
        else if (action->data() == "clear")
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(action->icon().pixmap(32, 32)));
            m_mediator->setMode(EditorMode::SetTileType);
        }
        // The user wants to set a tile type or clear it.
        else if (m_objectModelLoader->getCategoryByRole(
                   action->data().toString())
                 == "tile")
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(action->icon().pixmap(32, 32)));
            m_mediator->setMode(EditorMode::SetTileType);
        }
        // The user wants to add an object to the scene.
        else if (m_objectModelLoader->getCategoryByRole(
                   action->data().toString())
                 == "free")
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(QPixmap(":/cursor2.png")));
            m_mediator->setMode(EditorMode::AddObject);
        }
    }
    else
    {
        clearEditMode();
        m_currentToolBarAction = nullptr;
    }
}

void MainWindow::openTrack()
{
    clearEditMode();

    // Load recent path
    QSettings settings;

    settings.beginGroup(m_settingsGroup);
    QString path = settings.value("recentPath",
                                  QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
                     .toString();
    settings.endGroup();

    const QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open a track"), path, tr("Track Files (*.trk)"));
    if (!fileName.isEmpty())
    {
        doOpenTrack(fileName);
    }
}

void MainWindow::showAboutDlg()
{
    m_aboutDlg->exec();
}

void MainWindow::showAboutQtDlg()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::clearRoute()
{
    m_mediator->clearRoute();

    m_clearRouteAction->setEnabled(false);
}

bool MainWindow::doOpenTrack(QString fileName)
{
    if (!QFile::exists(fileName))
    {
        console(QString(tr("ERROR!!: %1 doesn't exist.").arg(fileName)));
        return false;
    }

    // Undo stack will be cleared.
    m_undoAction->setEnabled(false);
    m_redoAction->setEnabled(false);

    if (m_mediator->openTrack(fileName))
    {
        console(QString(tr("Track '%1' opened.").arg(fileName)));

        setTitle(fileName);

        // Save recent path
        QSettings settings;

        settings.beginGroup(m_settingsGroup);
        settings.setValue("recentPath", fileName);
        settings.endGroup();

        m_saveAction->setEnabled(true);

        setActionStatesOnNewTrack();

        fitScale();

        m_saved = true;

        m_clearRouteAction->setEnabled(m_mediator->routeHasNodes());

        return true;
    }

    console(QString(tr("Failed to open track '")) + fileName + "'.");
    return false;
}

void MainWindow::beginSetRoute()
{
    if (m_mediator->beginSetRoute())
    {
        console(tr("Set route: begin."));
        QMessageBox::information(
          this,
          tr("Set route, checkpoints and driving lines."),
          tr("Setting the route defines checkpoints for the cars so\n"
             "that no shortcuts can be taken. It also defines\n"
             "driving lines for the computer players.\n\n"
             "Click on the tiles one by one and make a closed loop\n"
             "with the target nodes. You can adjust the nodes afterwads.\n"
             "Start from the first tile after the finish line tile\n"
             "to make the lap detection and timing work correctly.\n"
             "Click on the first node again to finish."));
    }
    else
    {
        console(tr("Set route: not a valid track."));
        QMessageBox::critical(this, tr("Set route"), tr("Invalid track. Route cannot be set."));
    }
}

void MainWindow::clearEditMode()
{
    m_mediator->clearEditMode();
}

void MainWindow::console(QString text)
{
    QDateTime date = QDateTime::currentDateTime();
    m_console->append(QString("(") + date.toString("hh:mm:ss") + "): " + text);
}

void MainWindow::enableUndo(bool enable)
{
    m_undoAction->setEnabled(enable);
}

void MainWindow::endSetRoute()
{
    m_mediator->endSetRoute();

    m_clearRouteAction->setEnabled(m_mediator->routeHasNodes());

    console(tr("Set route: route finished."));
}

void MainWindow::fitScale()
{
    m_scaleSlider->setValue(m_mediator->fitScale());
}

void MainWindow::initializeNewTrack()
{
    const auto trackData = m_mediator->initializeNewTrack();
    if (std::get<bool>(trackData))
    {
        // Undo stack has been cleared.
        m_undoAction->setEnabled(false);
        m_redoAction->setEnabled(false);

        fitScale();

        setActionStatesOnNewTrack();

        setTitle(tr("New file"));

        m_clearRouteAction->setEnabled(false);

        m_saved = false;

        console(QString(tr("A new track '%1' created. Columns: %2, Rows: %3."))
                  .arg(std::get<1>(trackData))
                  .arg(std::get<2>(trackData))
                  .arg(std::get<3>(trackData)));
    }

    QApplication::restoreOverrideCursor();
}

void MainWindow::saveTrack()
{
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

    if (!m_saved)
    {
        saveAsTrack();
    }
    else
    {
        if (m_mediator->saveTrackData())
        {
            console(QString(tr("Track '")) + m_mediator->currentFileName() + tr("' saved."));
            setTitle(m_mediator->currentFileName());
            m_saved = true;
        }
        else
        {
            console(QString(tr("Failed to save track '")) + m_mediator->currentFileName() + "'.");
        }
    }

    QApplication::restoreOverrideCursor();
}

void MainWindow::saveAsTrack()
{
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save a track"),
                                                    QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                                    tr("Track Files (*.trk)"));

    const QString trackFileExtension(".trk");
    if (!fileName.endsWith(trackFileExtension))
    {
        fileName += trackFileExtension;
    }

    if (m_mediator->saveTrackDataAs(fileName))
    {
        console(QString(tr("Track '")) + fileName + tr("' saved."));
        setTitle(fileName);
        m_saved = true;
    }
    else
    {
        console(QString(tr("Failed to save track as '")) + fileName + "'.");
    }

    QApplication::restoreOverrideCursor();
}

void MainWindow::setupTrackAfterUndoOrRedo()
{
    m_saveAction->setEnabled(true);

    setActionStatesOnNewTrack();

    m_mediator->setupTrackAfterUndoOrRedo();

    m_clearRouteAction->setEnabled(m_mediator->routeHasNodes());
}

void MainWindow::setActionStatesOnNewTrack()
{
    m_saveAction->setEnabled(true);
    m_saveAsAction->setEnabled(true);
    m_toolBar->setEnabled(true);
    m_setRouteAction->setEnabled(true);
    m_setTrackPropertiesAction->setEnabled(true);
}

void MainWindow::setTrackProperties()
{
    if (m_mediator->setTrackProperties())
    {
        console(QString(tr("Track properties updated.")));
    }
}

MainWindow::~MainWindow() = default;
