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
#include "mcsurface.hh"
#include "mcsurfaceconfigloader.hh"
#include "mcsurfacemanager.hh"

#include <QDir>
#include <QGLWidget>
#include <MCGLEW>

#include <cassert>

MCSurfaceManager::MCSurfaceManager()
{
}

inline bool colorMatch(int val1, int val2, int threshold)
{
    return (val1 >= val2 - threshold) && (val1 <= val2 + threshold);
}

MCSurface & MCSurfaceManager::createSurfaceFromImage(
    const MCSurfaceMetaData & data, const QImage & image) throw (MCException)
{
    // Store original width of the image
    int origH = data.height.second ? data.height.first : image.height();
    int origW = data.width.second  ? data.width.first  : image.width();

    GLuint textureHandle = create2DTextureFromImage(data, image);

    // Create a new MCSurface object
    MCSurface * pSurface =
        new MCSurface(textureHandle, 0, origW, origH, data.z0, data.z1, data.z2, data.z3);

    // Enable alpha blend, if set
    pSurface->setAlphaBlend(data.alphaBlend.second, data.alphaBlend.first.m_src, data.alphaBlend.first.m_dst);

    // Set custom center if it was set
    if (data.center.second)
    {
        pSurface->setCenter(data.center.first);
    }

    // Store MCSurface to map
    m_surfaceMap[data.handle] = pSurface;

    assert(pSurface);
    return *pSurface;
}

MCSurface & MCSurfaceManager::createSurfaceFromImages(
    const MCSurfaceMetaData & data, const QImage & image1, const QImage & image2) throw (MCException)
{
    // Store original width of the image
    int origH = data.height.second ? data.height.first : image1.height();
    int origW = data.width.second  ? data.width.first  : image1.width();

    GLuint textureHandle1 = create2DTextureFromImage(data, image1);
    GLuint textureHandle2 = create2DTextureFromImage(data, image2);

    // Create a new MCSurface object
    MCSurface * pSurface =
        new MCSurface(textureHandle1, textureHandle2, origW, origH, data.z0, data.z1, data.z2, data.z3);

    // Enable alpha blend, if set
    pSurface->setAlphaBlend(
        data.alphaBlend.second, data.alphaBlend.first.m_src, data.alphaBlend.first.m_dst);

    // Set custom center if it was set
    if (data.center.second)
    {
        pSurface->setCenter(data.center.first);
    }

    // Store MCSurface to map
    m_surfaceMap[data.handle] = pSurface;

    assert(pSurface);
    return *pSurface;
}

GLuint MCSurfaceManager::create2DTextureFromImage(
    const MCSurfaceMetaData & data, const QImage & image)
{
    QImage textureImage = image;

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
        applyColorKey(textureImage, data.colorKey.m_r, data.colorKey.m_g, data.colorKey.m_b);
    }

    // Convert to GL_RGBA
    textureImage = QGLWidget::convertToGLFormat(textureImage);

    // Let OpenGL generate a texture handle
    GLuint textureHandle;
    glGenTextures(1, &textureHandle);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    // Set min filter.
    if (data.minFilter.second)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.minFilter.first);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    // Set mag filter.
    if (data.magFilter.second)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, data.magFilter.first);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    // Edit image data using the information textureImage gives us
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        textureImage.width(), textureImage.height(),
        0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    return textureHandle;
}

void MCSurfaceManager::applyColorKey(QImage & textureImage, MCUint r, MCUint g, MCUint b) const
{
    for (int i = 0; i < textureImage.width(); i++)
    {
        for (int j = 0; j < textureImage.height(); j++)
        {
            if (colorMatch( textureImage.pixel(i, j) & 0x000000ff,        b, 2) &&
                colorMatch((textureImage.pixel(i, j) & 0x0000ff00) >> 8,  g, 2) &&
                colorMatch((textureImage.pixel(i, j) & 0x00ff0000) >> 16, r, 2))
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

MCSurfaceManager::~MCSurfaceManager()
{
    // Delete OpenGL textures and Textures
    auto iter(m_surfaceMap.begin());
    while (iter != m_surfaceMap.end())
    {
        if (iter->second)
        {
            MCSurface * p = iter->second;

            GLuint dummyHandle1 = p->handle1();
            glDeleteTextures(1, &dummyHandle1);

            GLuint dummyHandle2 = p->handle2();
            if (dummyHandle2)
            {
                glDeleteTextures(1, &dummyHandle2);
            }

            delete p;
        }
        iter++;
    }
}

void MCSurfaceManager::load(
    const std::string & configFilePath, const std::string & baseDataPath) throw (MCException)
{
    MCSurfaceConfigLoader loader;

    // Parse the texture config file
    if (loader.load(configFilePath))
    {
        for (unsigned int i = 0; i < loader.surfaceCount(); i++)
        {
            const MCSurfaceMetaData & data = loader.surface(i);

            const std::string path =
                baseDataPath + QDir::separator().toLatin1() + data.imagePath1;

            // Create a 3D texture if data.imagePath2 is set.
            if (!data.imagePath2.empty())
            {
                const std::string path2 =
                    baseDataPath + QDir::separator().toLatin1() + data.imagePath2;

                // Load the images and create a multitexture.
                QImage textureImage1;
                if (textureImage1.load(path.c_str()))
                {
                    QImage textureImage2;
                    if (textureImage2.load(path2.c_str()))
                    {
                        createSurfaceFromImages(data, textureImage1, textureImage2);
                    }
                    else
                    {
                        throw MCException("Cannot read file '" + path2 + "'");
                    }
                }
                else
                {
                    throw MCException("Cannot read file '" + path + "'");
                }
            }
            else
            {
                // Load the image and create a 2D texture.
                QImage textureImage;
                if (textureImage.load(path.c_str()))
                {
                    createSurfaceFromImage(data, textureImage);
                }
                else
                {
                    throw MCException("Cannot read file '" + path + "'");
                }
            }
        }
    }
    else
    {
        // Throw an exception
        throw MCException("Parsing '" + configFilePath + "' failed!");
    }
}

MCSurface & MCSurfaceManager::surface(const std::string & id) const throw (MCException)
{
    // Try to find existing texture for the surface
    if (m_surfaceMap.count(id) == 0)
    {
        throw MCException("Cannot find texture object for handle '" + id + "'");
    }

    // Yes: return handle for the texture
    MCSurface * pSurface = m_surfaceMap.find(id)->second;
    assert(pSurface);
    return *pSurface;
}
