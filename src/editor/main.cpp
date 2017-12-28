// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include <QApplication>
#include <QSettings>

#include "../common/config.hpp"
#include "../common/userexception.hpp"

#include "application.hpp"

#include <iostream>
#include <memory>
#include <cstdlib>

#if QT_VERSION < 0x50600
extern Q_CORE_EXPORT QBasicAtomicInt qt_qhash_seed;
#endif

int main(int argc, char ** argv)
{
#if QT_VERSION >= 0x50600
    qSetGlobalQHashSeed(0);
#else
    qt_qhash_seed.store(0);
#endif

    QApplication::setOrganizationName(Config::Common::QSETTINGS_COMPANY_NAME);
    QApplication::setApplicationName(Config::Editor::QSETTINGS_SOFTWARE_NAME);
#ifdef Q_OS_WIN32
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    std::unique_ptr<Application> app;

    try
    {
        app.reset(new Application(argc, argv));

        return app->run();
    }
    catch (std::exception & e)
    {
        if (!dynamic_cast<UserException *>(&e))
        {
            std::cerr << e.what() << std::endl;
        }

        app.reset();

        return EXIT_FAILURE;
    }
}
