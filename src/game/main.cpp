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

#include <QApplication>
#include <QDir>

#include "game.h"
#include "mainwindow.h"
#include "renderer.h"

#include "MiniCore/Core/MCLogger"

int main(int argc, char ** argv)
{
    // Create the QApplication
    QApplication::setGraphicsSystem("raster");
    QApplication app(argc, argv);

    // Initialize the logger
    QString logPath = QDir::tempPath() + QDir::separator() + "dustrac.log";
    MCLogger::init(logPath.toStdString().c_str());
    MCLogger::setEchoMode(true);
    MCLogger::setDateTime(true);

    // Create the game object and set the renderer
    MCLogger::logInfo("Creating game..");
    Game game;
    game.setRenderer(new Renderer);
    game.setTargetFps(30);
    game.start();

    // Create the main window
    MCLogger::logInfo("Creating window..");
    MainWindow mainWindow;
    mainWindow.setCentralWidget(game.renderer());
    mainWindow.show();

    return app.exec();
}
