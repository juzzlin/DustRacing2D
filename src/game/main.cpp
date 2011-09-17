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

#include "game.h"
#include "mainwindow.h"
#include "renderer.h"

int main(int argc, char ** argv)
{
    QApplication::setGraphicsSystem("raster");
    QApplication app(argc, argv);

    Game game;
    game.setRenderer(new Renderer);
    game.setTargetFps(30);
    game.start();

    MainWindow mainWindow;
    mainWindow.setCentralWidget(game.renderer());
    mainWindow.show();

    return app.exec();
}
