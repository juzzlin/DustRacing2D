// This file belongs to the "Dust" car racing game
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mainwindow.h"
#include "editor.h"
#include "editorview.h"
#include "editorscene.h"
#include "newtrackdialog.h"
#include "trackdata.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QGraphicsLineItem>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QDateTime>

namespace 
{
    const char *       SETTINGS_GROUP = "MainWindow";
    const unsigned int TILE_W         = 256;
    const unsigned int TILE_H         = 256;
    const int          MARGIN         = 16;
}

MainWindow::MainWindow() :
        m_editor(new Editor),
        m_editorView(new EditorView(this)),
        m_editorScene(new EditorScene(this)),
        m_trackData(NULL),
        m_console(new QTextEdit(this))
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

    // Create layout for console and view
    QVBoxLayout * centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_editorView);

    // Add console to the layout
    m_console->setReadOnly(true);
    m_console->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    centralLayout->addWidget(m_console);

    // Create a new central widget and set the layout
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(centralLayout);

    console(tr("Choose 'File -> New' or 'File -> Open' to start.."));
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

    // Add "save" -action
    QAction * saveAct = new QAction(tr("&Save"), this);
    fileMenu->addAction(saveAct);

    // Add "save as" -action
    QAction * saveAsAct = new QAction(tr("&Save as..."), this);
    fileMenu->addAction(saveAsAct);

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

void MainWindow::initializeNewTrack()
{
    // Show a dialog asking some questions about the track
    NewTrackDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        const unsigned int newHorSize = dialog.horSize();
        const unsigned int newVerSize = dialog.verSize();

        delete m_trackData;
        m_trackData = new TrackData(dialog.name(), newHorSize, newVerSize);

        delete m_editorScene;
        m_editorScene = new EditorScene;

        QRectF newSceneRect(-MARGIN, -MARGIN,
                            2 * MARGIN + newHorSize * TILE_W, 2 * MARGIN + newVerSize  * TILE_H);

        m_editorScene->setSceneRect(newSceneRect);

        m_editorView->setScene(m_editorScene);
        m_editorView->setSceneRect(newSceneRect);

        createGrid();

        console(QString(tr("A new track '%1' created. Hor size: %2, ver size: %3."))
                .arg(m_trackData->name())
                .arg(m_trackData->horSize())
                .arg(m_trackData->verSize()));
    }
}

void MainWindow::createGrid()
{
    // Vertical lines
    for (unsigned int i = 0; i <= m_trackData->horSize(); i++)
        m_editorScene->addLine(0, 0, 0, m_trackData->verSize() * TILE_H)->translate(i * TILE_W, 0);

    // Horizontal lines
    for (unsigned int j = 0; j <= m_trackData->verSize(); j++)
        m_editorScene->addLine(0, 0, m_trackData->horSize() * TILE_W, 0)->translate(0, j * TILE_H);
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
