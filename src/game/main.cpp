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

#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QSettings>

#include "../common/config.hpp"
#include "../common/userexception.hpp"

#include "application.hpp"
#include "game.hpp"

#include "simple_logger.hpp"

#include <iostream>
#include <string>
#include <vector>

using juzzlin::L;

static void initLogger()
{
    QString logPath = QDir::tempPath() + QDir::separator() + "dr2d.log";
    L::initialize(logPath.toStdString().c_str());
    L::setTimestampMode(L::TimestampMode::DateTime);
    L::setLevelSymbol(L::Level::Trace, "<T>");
    L::setLevelSymbol(L::Level::Debug, "<D>");
    L::setLevelSymbol(L::Level::Info, "<I>");
    L::setLevelSymbol(L::Level::Warning, "<W>");
    L::setLevelSymbol(L::Level::Fatal, "<F>");
    L::enableEchoMode(true);
    L().info() << "Dust Racing 2D version " << VERSION;
    L().info() << "Compiled against Qt version " << QT_VERSION_STR;
}

int main(int argc, char ** argv)
{
    QGuiApplication::setOrganizationName(Config::General::QT_ORGANIZATION_NAME);
    QGuiApplication::setApplicationName(Config::Game::QT_APPLICATION_NAME);
#ifdef Q_OS_WIN32
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    try
    {
        initLogger();
        return Game(argc, argv).run();
    } //
    catch (std::exception & e)
    {
        if (!dynamic_cast<UserException *>(&e))
        {
            L().fatal() << e.what();
            L().fatal() << "Initializing the game failed!";
        }
        return EXIT_FAILURE;
    }
}
