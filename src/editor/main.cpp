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

#include <cstdlib>
#include <iostream>

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

    QApplication::setOrganizationName(Config::General::QT_ORGANIZATION_NAME);
    QApplication::setApplicationName(Config::Editor::QT_APPLICATION_NAME);
#ifdef Q_OS_WIN32
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    try
    {
        return Application(argc, argv).run();
    } //
    catch (std::exception & e)
    {
        if (!dynamic_cast<UserException *>(&e))
        {
            std::cerr << e.what() << std::endl;
        }
        return EXIT_FAILURE;
    }
}
