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

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QSlider>
#include <QTextEdit>
#include <QTransform>
#include <QVBoxLayout>

namespace 
{
    const char *       SETTINGS_GROUP = "MainWindow";
    const unsigned int TILE_W         = 256;
    const unsigned int TILE_H         = 256;
    const int          MARGIN         = 0;
    const unsigned int MIN_ZOOM       = 0;
    const unsigned int MAX_ZOOM       = 200;
    const unsigned int INI_ZOOM       = 100;
}

MainWindow::MainWindow() :
        m_editor(new Editor),
        m_editorView(new EditorView(this)),
        m_editorScene(new EditorScene(this)),
        m_trackData(NULL),
        m_console(new QTextEdit(this)),
        m_saveAction(NULL),
        m_saveAsAction(NULL),
        m_zoomSlider(new QSlider(Qt::Horizontal, this))
{
    setWindowTitle(QString(EDITOR_NAME) + " " + EDITOR_VERSION);

    QSettings settings(QSETTINGS_COMPANY_NAME, QSETTINGS_SOFTWARE_NAME);

    // Read dialog size data
    settings.beginGroup(SETTINGS_GROUP);
    resize(settings.value("size", QSize(640, 480)).toSize());
    settings.endGroup();

    // Try to center the window.
    QRect geometry(QApplication::desktop()->availableGeometry());
    move(geometry.width() / 2 - width() / 2, geometry.height() / 2 - height() / 2);

    // Create menu
    createMenuBar();

    // Set scene to the view
    m_editorView->setScene(m_editorScene);
    m_editorView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    // Create layout for console and view
    QVBoxLayout * centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_editorView);

    // Add zoom slider to the layout
    m_zoomSlider->setRange(MIN_ZOOM, MAX_ZOOM);
    m_zoomSlider->setValue(INI_ZOOM);
    m_zoomSlider->setTracking(false);
    m_zoomSlider->setTickInterval(10);
    m_zoomSlider->setTickPosition(QSlider::TicksBothSides);
    connect(m_zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(updateZoom(int)));
    centralLayout->addWidget(m_zoomSlider);

    // Add console to the layout
    m_console->setReadOnly(true);
    m_console->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    centralLayout->addWidget(m_console);

    // Create a new central widget and set the layout
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(centralLayout);

    console(tr("Choose 'File -> New' or 'File -> Open' to start.."));
}

void MainWindow::updateZoom(int value)
{
    qreal scale = static_cast<qreal>(value) / MAX_ZOOM;

    QTransform transform;
    transform.scale(scale, scale);
    m_editorView->setTransform(transform);

    console(QString("Zoom set to %1%").arg(value));
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

void MainWindow::createMenuBar()
{
    // Create "file" -menu
    QMenu * fileMenu = menuBar()->addMenu(tr("&File"));

    // Add "new" -action
    QAction * newAct = new QAction(tr("&New..."), this);
    fileMenu->addAction(newAct);
    connect(newAct, SIGNAL(triggered()), this, SLOT(initializeNewTrack()));

    // Add "open" -action
    QAction * openAct = new QAction(tr("&Open..."), this);
    fileMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(openTrack()));

    // Add "save" -action
    m_saveAction = new QAction(tr("&Save"), this);
    fileMenu->addAction(m_saveAction);
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(saveTrack()));
    m_saveAction->setEnabled(false);

    // Add "save as" -action
    m_saveAsAction = new QAction(tr("&Save as..."), this);
    fileMenu->addAction(m_saveAsAction);
    connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsTrack()));
    m_saveAsAction->setEnabled(false);

    // Add "quit" -action
    QAction * quitAct = new QAction(tr("&Quit"), this);
    fileMenu->addAction(quitAct);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Create "edit" -menu
    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));

    // Create "help" -menu
    QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about" -action
    QAction * aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    //  connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
}

void MainWindow::openTrack()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open a track"),
                                                    QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                                    tr("Track Files (*.trk)"));

    if (QFile::exists(fileName))
    {
        if (TrackIO::open(m_trackData, fileName))
        {
            console(QString(tr("Track '")) + fileName + tr("' opened."));

            m_saveAction->setEnabled(true);
            m_saveAsAction->setEnabled(true);
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
                            2 * MARGIN + cols * TILE_W, 2 * MARGIN + rows  * TILE_H);

        m_editorScene->setSceneRect(newSceneRect);
        m_editorView->setScene(m_editorScene);
        m_editorView->setSceneRect(newSceneRect);
        m_editorView->ensureVisible(0, 0, 0, 0);

        createGrid();

        m_saveAsAction->setEnabled(true);

        console(QString(tr("A new track '%1' created. Columns: %2, Rows: %3."))
                .arg(m_trackData->name())
                .arg(m_trackData->cols())
                .arg(m_trackData->rows()));
    }
}

void MainWindow::createGrid()
{
    // Vertical lines
    for (unsigned int i = 0; i <= m_trackData->cols(); i++)
        m_editorScene->addLine(0, 0, 0, m_trackData->rows() * TILE_H)->translate(i * TILE_W, 0);

    // Horizontal lines
    for (unsigned int j = 0; j <= m_trackData->rows(); j++)
        m_editorScene->addLine(0, 0, m_trackData->cols() * TILE_W, 0)->translate(0, j * TILE_H);
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
