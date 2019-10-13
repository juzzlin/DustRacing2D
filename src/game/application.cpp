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

#include "application.hpp"
#include "simple_logger.hpp"

#include <exception>

namespace {
static const char * INIT_ERROR = "Initializing the game failed!";
}

Application::Application(int & argc, char ** argv)
  : QApplication(argc, argv)
{
}

bool Application::notify(QObject * receiver, QEvent * event)
{
    try
    {
        return QApplication::notify(receiver, event);
    } catch (std::exception & e)
    {
        juzzlin::L().fatal() << e.what();
        juzzlin::L().fatal() << INIT_ERROR;

        QApplication::exit(EXIT_FAILURE);

        return false;
    }
}
