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

#include "mctextureparser.hh"
#include <QDir>
#include <iostream>

using std::cerr;
using std::endl;

MCTextureParser::MCTextureParser() :
  m_colorKeySet(false),
  m_xAxisMirror(false),
  m_strDefaultPath("")
{}

bool MCTextureParser::fatalError(const QXmlParseException & exception)
{
  qWarning() << "Fatal error on line" << exception.lineNumber()
      << ", column" << exception.columnNumber() << ":"
      << exception.message();

  return false;
}

QList<MCTextureData> MCTextureParser::textureData() const
{
  return m_textureData;
}

bool MCTextureParser::startDocument()
{
  return true;
}

bool MCTextureParser::endDocument()
{
  return true;
}

void MCTextureParser::elementError(const QString & qName)
{
  cerr << "ERROR!!: MCTextureParser: syntax error in element '" << qName.toStdString() << "'.." << endl;
}

bool MCTextureParser::startElement(const QString & namespaceURI,
                                   const QString & localName,
                                   const QString & qName,
                                   const QXmlAttributes & atts)
{
  // Fix a compiler warnings
  Q_UNUSED(localName);
  Q_UNUSED(namespaceURI);

  QString strVersion = "";

  // Match to a textures -tag
  if (qName == "textures")
  {
    // Clear data, if any
    m_textureData.clear();

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
  else if (qName == "texture")
  {
    // Create a new texture
    MCTextureData myTexture;
    
    // Read handle
    int index = atts.index("handle");
    if (index != -1)
    {
      myTexture.strHandle = atts.value(index);
    }
    else
    {
      elementError(qName);
      return false;
    }

    // Read image path
    index = atts.index("file");
    if (index != -1)
    {
      myTexture.strImage = m_strDefaultPath + QDir::separator() + atts.value(index);
    }
    else
    {
      elementError(qName);
      return false;
    }

    // Set mirroring flag
    myTexture.xAxisMirror = m_xAxisMirror;
    
    // Set colorkey if set
    if (m_colorKeySet)
    {
      myTexture.colorKey    = m_colorKey;
      myTexture.colorKeySet = true;
    }   

    // Read center x if set
    index = atts.index("x");
    if (index != -1)
    {
      myTexture.center.setI(atts.value(index).toInt());
      myTexture.centerSet = true;
    }

    // Read center y if set
    index = atts.index("y");
    if (index != -1)
    {
      myTexture.center.setJ(atts.value(index).toInt());
      myTexture.centerSet = true;
    }

    // Read width if set
    index = atts.index("w");
    if (index != -1)
    {
      myTexture.width = atts.value(index).toInt();
      myTexture.widthSet = true;
    }

    // Read height if set
    index = atts.index("h");
    if (index != -1)
    {
      myTexture.height = atts.value(index).toInt();
      myTexture.heightSet = true;
    }

    // Add texture to the vector
    m_textureData << myTexture;
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
  else if (qName == "x_axis_mirror")
  {
    // Read mirroring flag
    int index = atts.index("value");
    if (index != -1)
    {
      m_xAxisMirror = atts.value(index).toUInt();
    }
    else
    {
      elementError(qName);
      return false;
    }
  }
  else if (qName == "colorkey")
  {
    // Read r
    int index = atts.index("r");
    if (index != -1)
    {
      m_colorKey.r = atts.value(index).toUInt();
    }
    else
    {
      elementError(qName);
      return false;
    }

    // Read g
    index = atts.index("g");
    if (index != -1)
    {
      m_colorKey.g = atts.value(index).toUInt();
    }
    else
    {
      elementError(qName);
      return false;
    }

    // Read b
    index = atts.index("b");
    if (index != -1)
    {
      m_colorKey.b = atts.value(index).toUInt();
    }
    else
    {
      elementError(qName);
      return false;
    }

    m_colorKeySet = true;
  }
  else
  {
    cerr << "WARNING!!: MCTextureParser: ignoring unknown element '" << qName.toStdString() << "'.." << endl;
  }

  return true;
}

bool MCTextureParser::endElement(const QString &, const QString &, const QString &)
{
  return true;
}
