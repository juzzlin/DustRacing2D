// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include <QDir>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>

#include "../common/config.hpp"
#include "application.hpp"
#include "game.hpp"
#include "userexception.hpp"

#include <MCLogger>

#include <iostream>
#include <memory>
#include <vector>
#include <string>

static const char * INIT_ERROR = "Initializing the game failed!";

static void initLogger()
{
    QString logPath = QDir::tempPath() + QDir::separator() + "dustrac.log";
    MCLogger::init(logPath.toStdString().c_str());
    MCLogger::setEchoMode(true);
    MCLogger::setDateTime(true);
    MCLogger().info() << "Dust Racing 2D version " << VERSION;
    MCLogger().info() << "Compiled against Qt version " << QT_VERSION_STR;
}

int main(int argc, char ** argv)
{
    QApplication::setOrganizationName(Config::Common::QSETTINGS_COMPANY_NAME);
    QApplication::setApplicationName(Config::Game::QSETTINGS_SOFTWARE_NAME);
#ifdef Q_OS_WIN32
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    std::unique_ptr<Game> game;

    try
    {
        initLogger();

        // Create the main game object. The game loop starts immediately after
        // the Renderer has been initialized.
        MCLogger().info() << "Creating game object..";

        game.reset(new Game(argc, argv));

        return game->run();
    }
    catch (std::exception & e)
    {
        if (!dynamic_cast<UserException *>(&e))
        {
            MCLogger().fatal() << e.what();
            MCLogger().fatal() << INIT_ERROR;
        }

        game.reset();

        return EXIT_FAILURE;
    }
}

