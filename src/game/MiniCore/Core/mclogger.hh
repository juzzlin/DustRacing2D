// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MCLOGGER_HH
#define MCLOGGER_HH

#include <cstdio>
#include <cstdarg>

//! A logging interface.
class MCLogger
{
public:

    //! Initialize the logger.
    //! \param fileName Log to fileName. Can be nullptr.
    //! \param append The existing log will be appended if true.
    //! \return false if file couldn't be opened.
    static bool init(const char * fileName, bool append = false);

    //! Enable/disable echo mode.
    //! \param enable Echo everything if true. Default is false.
    static void setEchoMode(bool enable);

    //! Enable/disable date and time prefix.
    //! \param enable Prefix with date and time if true. Default is true.
    static void setDateTime(bool enable);

    //! Log an info message. Logs only to stdout if
    //! init failed and echoMode is enabled.
    //! \param format The printf-styled message.
    static void logInfo(const char * format, ...);

    //! Log a warning message. Logs only to stderr if
    //! init failed and echoMode is enabled.
    //! \param format The printf-styled message.
    static void logWarning(const char * format, ...);

    //! Log an error message. Logs only to stderr if
    //! init failed and echoMode is enabled.
    //! \param format The printf-styled message.
    static void logError(const char * format, ...);

    //! Log a fatal message. Logs only to stderr if
    //! init failed and echoMode is enabled.
    //! \param format The printf-styled message.
    static void logFatal(const char * format, ...);

private:

    //! Disdabled constructor.
    MCLogger();

    static void prefixDateTime();
    static void doLog(const char * type, const char * format, va_list ap);

    static bool   m_echoMode;
    static bool   m_dateTime;
    static FILE * m_stream;
};

#endif // MCLOGGER_HH
