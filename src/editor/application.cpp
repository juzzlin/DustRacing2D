// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#include "application.hpp"
#include "argengine.hpp"
#include "mainwindow.hpp"

#include "../common/config.hpp"
#include "../common/userexception.hpp"

#include <QLocale>
#include <QSettings>

#include <iostream>

static void initTranslations(QTranslator & appTranslator, QGuiApplication & app, QString lang = "")
{
    if (lang == "")
    {
        lang = QLocale::system().name();
    }

    if (appTranslator.load(QString(DATA_PATH) + "/translations/dustrac-editor_" + lang))
    {
        app.installTranslator(&appTranslator);
        std::cout << "Loaded translations for " << lang.toStdString() << std::endl;
    }
    else
    {
        std::cerr << "Failed to load translations for " << lang.toStdString() << std::endl;
    }
}

void Application::parseArgs(int argc, char ** argv)
{
    QString lang = "";

    juzzlin::Argengine ae(argc, argv);

    ae.addOption(
      { "--lang" }, [&](std::string value) {
          lang = value.c_str();
      },
      false, "Force language: fi, fr, it, cs, nl, ru.");

    ae.setHelpText(std::string("Dust Racing 2D version ") + VERSION + "\n" + //
                   Config::General::COPYRIGHT + "\n\nUsage: " + std::string(argv[0]) + " [OPTIONS] [TRACK_FILE]");

    ae.setPositionalArgumentCallback([=](juzzlin::Argengine::ArgumentVector args) {
        m_trackFile = args.at(0).c_str();
    });

    ae.parse();

    initTranslations(m_appTranslator, m_app, lang);
}

Application::Application(int & argc, char ** argv)
  : m_app(argc, argv)
{
    parseArgs(argc, argv);

    m_mainWindow = new MainWindow(m_trackFile);
    m_mainWindow->show();
}

int Application::run()
{
    return m_app.exec();
}

Application::~Application()
{
    delete m_mainWindow;
}
