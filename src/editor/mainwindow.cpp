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

#include "mainwindow.h"
#include "editor.h"
#include "editorview.h"
#include "editorscene.h"
#include "newtrackdialog.h"
#include "trackdata.h"
#include "trackio.h"
#include "tracktile.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include <QMenu>
#include <QMenuBar>
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

MainWindow * MainWindow::m_instance = NULL;

namespace
{
    const char *       SETTINGS_GROUP = "MainWindow";
    const int          MARGIN         = 0;
    const unsigned int MIN_ZOOM       = 0;
    const unsigned int MAX_ZOOM       = 200;
    const unsigned int INI_ZOOM       = 100;
    const int          CONSOLE_HEIGHT = 64;
    const char *       STRAIGHT_PATH  = ":/data/images/straight.png";
    const char *       CORNER_PATH    = ":/data/images/corner.png";
    const char *       GRASS_PATH     = ":/data/images/grass.png";
    const char *       CLEAR_PATH     = ":/data/images/clear.png";
}

MainWindow::MainWindow() :
        m_editor(new Editor),
        m_editorView(new EditorView(this)),
        m_editorScene(new EditorScene(this)),
        m_trackData(NULL),
        m_console(new QTextEdit(this)),
        m_saveAction(NULL),
        m_saveAsAction(NULL),
        m_currentToolBarAction(NULL),
        m_scaleSlider(new QSlider(Qt::Horizontal, this)),
        m_toolBar(new QToolBar(this))
{
    if (!m_instance)
    {
        m_instance = this;
    }
    else
    {
        qFatal("MainWindow already instantiated!");
    }

    setWindowTitle(QString(EDITOR_NAME) + " " + EDITOR_VERSION);

    QSettings settings(QSETTINGS_COMPANY_NAME, QSETTINGS_SOFTWARE_NAME);

    // Read dialog size data
    settings.beginGroup(SETTINGS_GROUP);
    resize(settings.value("size", QSize(640, 480)).toSize());
    settings.endGroup();

    // Try to center the window.
    QRect geometry(QApplication::desktop()->availableGeometry());
    move(geometry.width() / 2 - width() / 2, geometry.height() / 2 - height() / 2);

    // Populate menu bar with actions
    populateMenuBar();

    // Set scene to the view
    m_editorView->setScene(m_editorScene);
    m_editorView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
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

    // Print a welcome message
    console(tr("Choose 'File -> New' or 'File -> Open' to start.."));
}

MainWindow * MainWindow::instance()
{
    return MainWindow::m_instance;
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
    QSettings settings(QSETTINGS_COMPANY_NAME, QSETTINGS_SOFTWARE_NAME);

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
    fileMenu->addAction(newAct);
    connect(newAct, SIGNAL(triggered()), this, SLOT(initializeNewTrack()));

    // Add "open"-action
    QAction * openAct = new QAction(tr("&Open..."), this);
    fileMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(openTrack()));

    // Add "save"-action
    m_saveAction = new QAction(tr("&Save"), this);
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(saveTrack()));
    m_saveAction->setEnabled(false);

    // Add "save as"-action
    m_saveAsAction = new QAction(tr("&Save as..."), this);
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsTrack()));
    m_saveAsAction->setEnabled(false);

    // Add "quit"-action
    QAction * quitAct = new QAction(tr("&Quit"), this);
    fileMenu->addAction(quitAct);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Create "edit"-menu
    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));

    // Add "clear"-action
    QAction * clearAct = new QAction(tr("&Clear"), this);
    editMenu->addAction(clearAct);
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clear()));

    // Create "help"-menu
    QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about"-action
    QAction * aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    //  connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
}

void MainWindow::populateToolBar()
{
    // Add "straight"-action
    QAction * p = new QAction(QIcon(QPixmap(STRAIGHT_PATH)), tr("Straight"), this);
    p->setData(QVariant(QString("straight")));
    m_toolBar->addAction(p);

    // Add "corner"-action
    p = new QAction(QIcon(QPixmap(CORNER_PATH)), tr("Corner"), this);
    p->setData(QVariant(QString("corner")));
    m_toolBar->addAction(p);

    // Add "grass"-action
    p = new QAction(QIcon(QPixmap(GRASS_PATH)), tr("Grass"), this);
    p->setData(QVariant(QString("grass")));
    m_toolBar->addAction(p);

    // Add "clear"-action
    p = new QAction(QIcon(QPixmap(CLEAR_PATH)), tr("Clear"), this);
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

        if (action->data() == "straight")
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(QPixmap(STRAIGHT_PATH)
                                                    .scaled(QSize(32, 32))));
        }
        else if (action->data() == "corner")
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(QPixmap(CORNER_PATH)
                                                    .scaled(QSize(32, 32))));
        }
        else if (action->data() == "grass")
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(QPixmap(GRASS_PATH)
                                                    .scaled(QSize(32, 32))));
        }
        else if (action->data() == "clear")
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(QCursor(QPixmap(CLEAR_PATH)
                                                    .scaled(QSize(32, 32))));
        }
    }
    else
    {
        QApplication::restoreOverrideCursor();
        m_currentToolBarAction = NULL;
    }
}

void MainWindow::openTrack()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open a track"),
                                                    QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                                    tr("Track Files (*.trk)"));

    if (QFile::exists(fileName))
    {
        delete m_trackData;
        if ((m_trackData) = TrackIO::open(fileName))
        {
            console(QString(tr("Track '")) + fileName + tr("' opened."));

            m_saveAction->setEnabled(true);
            m_saveAsAction->setEnabled(true);
            m_toolBar->setEnabled(true);

            addTilesToScene();
        }
        else
        {
            console(QString(tr("Failed to open track '")) + fileName + "'.");
        }
    }
}

void MainWindow::saveTrack()
{
    if (TrackIO::save(m_trackData, m_trackData->fileName()))
    {
        console(QString(tr("Track '")) + m_trackData->fileName() + tr("' saved."));
    }
    else
    {
        console(QString(tr("Failed to save track '")) + m_trackData->fileName() + "'.");
    }
}

void MainWindow::saveAsTrack()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Open a track"),
                                                    QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                                    tr("Track Files (*.trk)"));

    if (!fileName.endsWith(".trk"))
    {
        fileName += ".trk";
    }

    if (TrackIO::save(m_trackData, fileName))
    {
        console(QString(tr("Track '")) + fileName + tr("' saved."));

        m_trackData->setFileName(fileName);
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

        delete m_trackData;
        m_trackData = new TrackData(dialog.name(), cols, rows);

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

        console(QString(tr("A new track '%1' created. Columns: %2, Rows: %3."))
                .arg(m_trackData->name())
                .arg(m_trackData->cols())
                .arg(m_trackData->rows()));
    }
}

void MainWindow::addTilesToScene()
{
    for (unsigned int i = 0; i < m_trackData->cols(); i++)
        for (unsigned int j = 0; j < m_trackData->rows(); j++)
        {
            if (TrackTile * tile = m_trackData->tile(i, j))
                m_editorScene->addItem(tile);
        }

    if (m_trackData->tile(0, 0))
        m_trackData->tile(0, 0)->setActive(true);
}

void MainWindow::clear()
{
    for (unsigned int i = 0; i < m_trackData->cols(); i++)
        for (unsigned int j = 0; j < m_trackData->rows(); j++)
        {
            if (TrackTile * p = m_trackData->tile(i, j))
                p->setTileType(TrackTile::TT_NONE);
        }

    m_console->append(QString(tr("Tiles cleared.")));
}

void MainWindow::console(QString text)
{
    QDateTime date = QDateTime::currentDateTime();
    m_console->append(QString("(") + date.toString("hh:mm:ss") + "): " + text);
}

MainWindow::~MainWindow()
{
    delete m_trackData;
    delete m_editor;
}
