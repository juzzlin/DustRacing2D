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

#include "mclogger.hh"
#include <QDateTime>
#include <cstdarg>
#include <cstdio>

bool   MCLogger::m_echoMode = false;
bool   MCLogger::m_dateTime = true;
FILE * MCLogger::m_stream   = NULL;

bool MCLogger::init(const char * fileName, bool append)
{
    MCLogger::m_stream = NULL;
    if (fileName)
    {
        if (append)
        {
            MCLogger::m_stream = fopen(fileName, "a+");
        }
        else
        {
            MCLogger::m_stream = fopen(fileName, "w+");
        }

        if (!m_stream)
        {
            fprintf(stderr, "ERROR!!: Couldn't open '%s' for write.\n", fileName);
            return false;
        }
    }

    return true;
}

void MCLogger::setEchoMode(bool enable)
{
    MCLogger::m_echoMode = enable;
}

void MCLogger::setDateTime(bool enable)
{
    MCLogger::m_dateTime = enable;
}

void MCLogger::prefixDateTime()
{
    if (MCLogger::m_dateTime)
    {
        const char * date = QDateTime::currentDateTime().toString().toStdString().c_str();
        fprintf(MCLogger::m_stream, "[%s] ", date);
        fprintf(stdout, "[%s] ", date);
    }
}

void MCLogger::logInfo(const char * format, ...)
{
    MCLogger::prefixDateTime();

    va_list ap;
    va_start(ap, format);

    if (MCLogger::m_stream)
    {
        fprintf(MCLogger::m_stream, "INFO: ");
        vfprintf(MCLogger::m_stream, format, ap);
        fprintf(MCLogger::m_stream, "\n");
        fflush(MCLogger::m_stream);
    }

    if (MCLogger::m_echoMode)
    {
        fprintf(stdout, "INFO: ");
        vfprintf(stdout, format, ap);
        fprintf(stdout, "\n");
    }

    va_end(ap);
}

void MCLogger::logWarning(const char * format, ...)
{
    MCLogger::prefixDateTime();

    va_list ap;
    va_start(ap, format);

    if (MCLogger::m_stream)
    {
        fprintf(MCLogger::m_stream, "INFO: ");
        vfprintf(MCLogger::m_stream, format, ap);
        fprintf(MCLogger::m_stream, "\n");
        fflush(MCLogger::m_stream);
    }

    if (MCLogger::m_echoMode)
    {
        fprintf(stdout, "INFO: ");
        vfprintf(stdout, format, ap);
        fprintf(stdout, "\n");
    }

    va_end(ap);
}

void MCLogger::logError(const char * format, ...)
{
    MCLogger::prefixDateTime();

    va_list ap;
    va_start(ap, format);

    if (MCLogger::m_stream)
    {
        fprintf(MCLogger::m_stream, "ERROR: ");
        vfprintf(MCLogger::m_stream, format, ap);
        fprintf(MCLogger::m_stream, "\n");
        fflush(MCLogger::m_stream);
    }

    if (MCLogger::m_echoMode)
    {
        fprintf(stderr, "ERROR: ");
        vfprintf(stderr, format, ap);
        fprintf(stderr, "\n");
    }

    va_end(ap);
}

void MCLogger::logFatal(const char * format, ...)
{
    MCLogger::prefixDateTime();

    va_list ap;
    va_start(ap, format);

    if (MCLogger::m_stream)
    {
        fprintf(MCLogger::m_stream, "FATAL: ");
        vfprintf(MCLogger::m_stream, format, ap);
        fprintf(MCLogger::m_stream, "\n");
        fflush(MCLogger::m_stream);
    }

    if (MCLogger::m_echoMode)
    {
        fprintf(stderr, "FATAL: ");
        vfprintf(stderr, format, ap);
        fprintf(stderr, "\n");
    }

    va_end(ap);
}

