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
#include "version.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QTimer>

MainWindow * MainWindow::m_instance = NULL;

namespace
{
    const char *       SETTINGS_GROUP = "MainWindow";
    const int          MARGIN         = 0;
    const unsigned int MIN_ZOOM       = 0;
    const unsigned int MAX_ZOOM       = 200;
    const unsigned int INI_ZOOM       = 100;
    const int          CONSOLE_HEIGHT = 64;
}

MainWindow::MainWindow()
{
    if (!m_instance)
    {
        m_instance = this;
    }
    else
    {
        qFatal("MainWindow already instantiated!");
    }

    setWindowTitle(QString(Version::GAME_NAME) + " " + Version::GAME_VERSION);

    QSettings settings(Version::QSETTINGS_COMPANY_NAME,
                       Version::QSETTINGS_SOFTWARE_NAME);

    // Read window size data
    settings.beginGroup(SETTINGS_GROUP);
    resize(settings.value("size", QSize(640, 480)).toSize());
    settings.endGroup();

    // Try to center the window.
    QRect geometry(QApplication::desktop()->availableGeometry());
    move(geometry.width() / 2 - width() / 2, geometry.height() / 2 - height() / 2);

    // Populate menu bar with actions
    populateMenuBar();
}

MainWindow * MainWindow::instance()
{
    return MainWindow::m_instance;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    // Open settings file
    QSettings settings(Version::QSETTINGS_COMPANY_NAME,
                       Version::QSETTINGS_SOFTWARE_NAME);

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

    // Add "quit"-action
    QAction * quitAct = new QAction(tr("&Quit"), this);
    fileMenu->addAction(quitAct);
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Create "help"-menu
    QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));

    // Add "about"-action
    QAction * aboutAct = new QAction(tr("&About"), this);
    helpMenu->addAction(aboutAct);
    //  connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
}

MainWindow::~MainWindow()
{
}
