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

#include <QApplication>
#include <QDir>
#include <QHBoxLayout>

#include "game.hpp"
#include "mainwindow.hpp"
#include "renderer.hpp"

#include <MCLogger>

int main(int argc, char ** argv)
{
    // Create the QApplication
    QApplication::setGraphicsSystem("opengl");
    QApplication app(argc, argv);

    // Initialize the logger
    QString logPath = QDir::tempPath() + QDir::separator() + "dustrac.log";
    MCLogger::init(logPath.toStdString().c_str());
    MCLogger::setEchoMode(true);
    MCLogger::setDateTime(true);

    MCLogger().info() << "Creating renderer..";
    Renderer * renderer = new Renderer;

    // Create the main window
    MCLogger().info() << "Creating the main window..";
    MainWindow mainWindow;
    QHBoxLayout * layout = new QHBoxLayout(&mainWindow);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(renderer);
    mainWindow.setContentsMargins(0, 0, 0, 0);
    mainWindow.setCursor(QCursor(Qt::BlankCursor));
    mainWindow.show();

    // Create the game object and set the renderer
    MCLogger().info() << "Creating game object..";
    Game game;
    game.setRenderer(renderer);
    game.setTargetUpdateFps(60);
    game.connect(&mainWindow, SIGNAL(closed()), &game, SLOT(finish()));

    // Initialize and start the game
    if (game.init())
    {
        game.start();
    }
    else
    {
        MCLogger().error() << "Initing the game failed.";
        return EXIT_FAILURE;
    }

    return app.exec();
}
