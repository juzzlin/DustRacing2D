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

#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>

namespace 
{
    const char * SETTINGS_GROUP = "MainWindow";
}

MainWindow::MainWindow() :
    m_editor(new Editor)
{
  QSettings settings(QSETTINGS_COMPANY_NAME, QSETTINGS_SOFTWARE_NAME);

  // Read dialog size data
  settings.beginGroup(SETTINGS_GROUP);
  resize(settings.value("size", QSize(640, 480)).toSize());
  settings.endGroup();

  // Try to center the window.
  QRect geometry(QApplication::desktop()->availableGeometry());
  move(geometry.width() / 2 - width() / 2, geometry.height() / 2 - height() / 2);

  createMenuBar();
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

  // Create "help" -menu
  QMenu * helpMenu = menuBar()->addMenu(tr("&Help"));

  // Add "about" -action
  QAction * aboutAct = new QAction(tr("&About"), this);
  helpMenu->addAction(aboutAct);
//  connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
}

MainWindow::~MainWindow()
{
    delete m_editor;
}
