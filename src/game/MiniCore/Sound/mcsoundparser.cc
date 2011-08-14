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

#include "mcsoundparser.hh"
#include <QDir>
#include <iostream>

using std::cerr;
using std::endl;

MCSoundParser::MCSoundParser() :
  m_strDefaultPath("")
{}

bool MCSoundParser::fatalError(const QXmlParseException & exception)
{
  qWarning() << "Fatal error on line" << exception.lineNumber()
      << ", column" << exception.columnNumber() << ":"
      << exception.message();

  return false;
}

QList<MCSoundData> MCSoundParser::soundData() const
{
  return m_vectSoundData;
}

bool MCSoundParser::startDocument()
{
  return true;
}

bool MCSoundParser::endDocument()
{
  return true;
}

void MCSoundParser::elementError(const QString & qName)
{
  cerr << "ERROR!!: MCSoundParser: syntax error in element '" << qName.toStdString() << "'.." << endl;
}

bool MCSoundParser::startElement(const QString & namespaceURI,
                                 const QString & localName,
                                 const QString & qName,
                                 const QXmlAttributes & atts)
{
  // Fix a compiler warnings
  Q_UNUSED(localName);
  Q_UNUSED(namespaceURI);

  QString strVersion = "";

  // Match to a sounds -tag
  if (qName == "sounds")
  {
    // Clear data, if any
    m_vectSoundData.clear();

    // Read version
    int index = atts.index("version");
    if (index != -1)
    {
      strVersion = atts.value(index);
    }
    else
    {
      elementError(qName);
      return false;
    }
  }
  else if (qName == "sound")
  {
    // Create a new MCSoundData
    MCSoundData mySound;
    
    // Read handle
    int index = atts.index("handle");
    if (index != -1)
    {
      mySound.strHandle = atts.value(index);
    }
    else
    {
      elementError(qName);
      return false;
    }

    // Read file path
    index = atts.index("file");
    if (index != -1)
    {
      mySound.strFile = (m_strDefaultPath + QDir::separator() + atts.value(index));
    }
    else
    {
      elementError(qName);
      return false;
    }

    // Add Sound to the vector
    m_vectSoundData.push_back(mySound);
  }
  else if (qName == "music")
  {
    // Create a new MCSoundData
    MCSoundData mySound;
    mySound.isMusic = true;

    // Read handle
    int index = atts.index("handle");
    if (index != -1)
    {
      mySound.strHandle = atts.value(index);
    }
    else
    {
      elementError(qName);
      return false;
    }

    // Read file path
    index = atts.index("file");
    if (index != -1)
    {
      mySound.strFile = (m_strDefaultPath + QDir::separator() + atts.value(index));
    }
    else
    {
      elementError(qName);
      return false;
    }

    // Add Sound to the vector
    m_vectSoundData << mySound;
  }
  else if (qName == "default_path")
  {
    // Read default path
    int index = atts.index("value");
    if (index != -1)
    {
      m_strDefaultPath = atts.value(index);
    }
    else
    {
      elementError(qName);
      return false;
    }
  }
  else
  {
    cerr << "WARNING!!: MCSoundParser: ignoring unknown element '" << qName.toStdString() << "'.." << endl;
  }

  return true;
}

bool MCSoundParser::endElement(const QString &, const QString &, const QString &)
{
  return true;
}
