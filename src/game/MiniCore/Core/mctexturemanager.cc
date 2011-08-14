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

#include "mctypes.hh"
#include "mctexturemanager.hh"
#include "mctextureparser.hh"
#include "mcsurface.hh"

#include <QDir>
#include <QGLWidget>
#include <GL/gl.h>

MCTextureManager::MCTextureManager() :
  m_mapTextures()
{}

void MCTextureManager::load(const QString & strFileName, const QString & strBaseDataPath) throw (MCException)
{
  // Instantiate a parser
  MCTextureParser handler;
  
  // Instantiate a QFile for the texture file
  QFile xmlFile(strFileName);

  // Set QXmlInputSource to the QFile
  QXmlInputSource source(&xmlFile);

  // Instantiate a simple (SAX2) reader
  QXmlSimpleReader reader;

  // Set it to use the MCTextureParser as the handler
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);

  // Parse the texture config file
  if (reader.parse(source))
  {
    // Get texture data / parameters
    QList<MCTextureData> textureDataList = handler.textureData();
    Q_FOREACH(MCTextureData data, textureDataList)
    {
      // Load image file
      QString path = strBaseDataPath + QDir::separator().toAscii() + data.strImage;

      // Load the image
      QImage textureImage;
      if (textureImage.load(path))
      {
        // Create an OpenGL texture from the image
        createGLTextureFromImage(data, textureImage);
      }
      else
      {
        throw MCException("Cannot read file '" + path + "'");
      }
    }
  }
  else
  {
    // Throw an exception
    throw MCException("Level::loadTextures(): parsing '" + strFileName + "' failed!");
  }
}

QImage MCTextureManager::createNearest2PowNImage(const QImage & image)
{
  int n = 0;
  int w = image.width();
  int h = image.height();

  n = log(static_cast<double>(w)) / log(2);
  w = pow(2, n);
  w = w < 2 ? 2 : w;
  n = log(static_cast<double>(h)) / log(2);
  h = pow(2, n);
  h = h < 2 ? 2 : h;

  return image.scaled(w, h);
}

inline bool colorMatch(int val1, int val2, int threshold)
{
  return (val1 >= val2 - threshold) && (val1 <= val2 + threshold);
}

void MCTextureManager::createGLTextureFromImage(const MCTextureData & data, const QImage & image)
{
  // Store original width of the image
  int origH = data.heightSet ? data.height : image.height();
  int origW = data.widthSet  ? data.width  : image.width();

  // Create a surface with dimensions of 2**n
  QImage textureImage = createNearest2PowNImage(image);

  // Flip pA about X-axis if set active
  if (data.xAxisMirror)
  {
    textureImage = textureImage.mirrored(false, true);
  }

  // Ensure alpha channel
  textureImage = textureImage.convertToFormat(QImage::Format_ARGB32);

  // Apply colorkey if it was set (set or clear alpha)
  if (data.colorKeySet)
  {
    for (int i = 0; i < textureImage.width(); i++)
    {
      for (int j = 0; j < textureImage.height(); j++)
      {
        if (colorMatch( textureImage.pixel(i, j) & 0x000000ff,        data.colorKey.b, 2) &&
            colorMatch((textureImage.pixel(i, j) & 0x0000ff00) >> 8,  data.colorKey.g, 2) &&
            colorMatch((textureImage.pixel(i, j) & 0x00ff0000) >> 16, data.colorKey.r, 2))
        {
          textureImage.setPixel(i, j, textureImage.pixel(i, j) & 0x00000000);
        }
        else
        {
          textureImage.setPixel(i, j, textureImage.pixel(i, j) | 0xff000000);
        }
      }
    }
  }

  // Convert to GL_RGBA
  textureImage = QGLWidget::convertToGLFormat(textureImage);

  // Let OpenGL generate a texture handle
  GLuint textureHandle;
  glGenTextures( 1, &textureHandle );

  // Bind the texture object
  glBindTexture( GL_TEXTURE_2D, textureHandle );

  // Set the texture's stretching properties
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Edit image data using the information SDL_Surface gives us
  glTexImage2D(GL_TEXTURE_2D, 0, 4, textureImage.width(), textureImage.height(), 0,
               GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

  // Create a new MCSurface object
  MCSurface * pTexture = NULL;
  if (data.centerSet)
  {
    pTexture = new MCSurface(textureHandle, origW, origH, data.center, data.colorKeySet);
  }
  else
  {
    pTexture = new MCSurface(textureHandle, origW, origH, data.colorKeySet);
  }

  // Store MCSurface to map
  m_mapTextures[data.strHandle] = pTexture;
}

MCSurface * MCTextureManager::surface(const QString & strId) const throw (MCException)
{
  // Try to find existing texture for the surface
  if (!m_mapTextures.contains(strId))
  {
    // No: 
    throw MCException("Cannot find texture object for handle '" + strId + "'");
    return NULL;
  }
  else
  {
    // Yes: return handle for the texture
    return m_mapTextures.find(strId).value();
  }
}

MCTextureManager::~MCTextureManager()
{
  // Delete OpenGL textures and Textures
  TextureHash::iterator iter(m_mapTextures.begin());
  while (iter != m_mapTextures.end())
  {
    if (iter.value())
    {
      MCSurface * p = iter.value();
      GLuint dummyHandle = p->handle();
      glDeleteTextures(1, &dummyHandle);
      delete p;
    }
    iter++;
  }
  m_mapTextures.clear();
}
