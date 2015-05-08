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
#include <QTranslator>

#include "../common/config.hpp"
#include "application.hpp"
#include "game.hpp"

#include <MCLogger>

#include <iostream>
#include <vector>
#include <string>

static void initLogger()
{
    QString logPath = QDir::tempPath() + QDir::separator() + "dustrac.log";
    MCLogger::init(logPath.toStdString().c_str());
    MCLogger::setEchoMode(true);
    MCLogger::setDateTime(true);
    MCLogger().info() << "Dust Racing 2D version " << VERSION;
    MCLogger().info() << "Compiled against Qt version " << QT_VERSION_STR;
}

static void printHelp()
{
    std::cout << std::endl << "Dust Racing 2D version " << VERSION << std::endl;
    std::cout << Config::Common::COPYRIGHT.toStdString() << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help        Show this help." << std::endl;
    std::cout << "--lang [lang] Force language: fi, fr, it, cs." << std::endl;
    std::cout << "--no-vsync    Force vsync off." << std::endl;
    std::cout << std::endl;
}

static void initTranslations(QTranslator & appTranslator, QGuiApplication & app, QString lang = "")
{
    if (lang == "")
    {
        lang = QLocale::system().name();
    }

    if (appTranslator.load(QString(DATA_PATH) + "/translations/dustrac-game_" + lang))
    {
        app.installTranslator(&appTranslator);
        MCLogger().info() << "Loaded translations for " << lang.toStdString();
    }
    else
    {
        MCLogger().warning() << "Failed to load translations for " << lang.toStdString();
    }
}

int main(int argc, char ** argv)
{
    QApplication::setOrganizationName(Config::Common::QSETTINGS_COMPANY_NAME);
    QApplication::setApplicationName(Config::Game::QSETTINGS_SOFTWARE_NAME);
#ifdef Q_OS_WIN32
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    Application app(argc, argv);
    QTranslator appTranslator;
    QString lang = "";

    bool forceNoVSync = false;
    std::vector<QString> args(argv, argv + argc);
    if (std::find(args.begin(), args.end(), "--help") != args.end())
    {
        printHelp();
        exit(EXIT_SUCCESS);
    }
    else
    {
        for (unsigned int i = 0; i < args.size(); i++)
        {
            if (args[i] == "--lang" && (i + i) < args.size())
            {
                lang = args[i + 1];
            }
            else if (args[i] == "--no-vsync")
            {
                forceNoVSync = true;
            }
        }
    }

    initLogger();
    initTranslations(appTranslator, app, lang);

    // Create the main game object. The game loop starts immediately after
    // the Renderer has been initialized.
    MCLogger().info() << "Creating game object..";
    Game game(forceNoVSync);

    return app.exec();
}
