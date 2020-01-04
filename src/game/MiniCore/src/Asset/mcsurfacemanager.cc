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

#include "mcsurfacemanager.hh"
#include "mclogger.hh"
#include "mcsurface.hh"
#include "mcsurfaceconfigloader.hh"

#include <MCGLEW>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QSysInfo>

#include <cassert>
#include <cmath>
#include <exception>

inline bool colorMatch(int val1, int val2, int threshold)
{
    return (val1 >= val2 - threshold) && (val1 <= val2 + threshold);
}

// This function is taken from Qt in order to drop dependency to QGLWidget::convertToGLFormat().
static inline QRgb qt_gl_convertToGLFormatHelper(QRgb src_pixel, GLenum texture_format)
{
#ifdef __MC_GLES__
    (void)texture_format;
    if (false)
    {
#else
    if (texture_format == GL_BGRA)
    {
#endif
        if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        {
            return ((src_pixel << 24) & 0xff000000)
              | ((src_pixel >> 24) & 0x000000ff)
              | ((src_pixel << 8) & 0x00ff0000)
              | ((src_pixel >> 8) & 0x0000ff00);
        }
        else
        {
            return src_pixel;
        }
    }
    else
    { // GL_RGBA
        if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        {
            return (src_pixel << 8) | ((src_pixel >> 24) & 0xff);
        }
        else
        {
            return ((src_pixel << 16) & 0xff0000)
              | ((src_pixel >> 16) & 0xff)
              | (src_pixel & 0xff00ff00);
        }
    }
}

// This function is taken from Qt in order to drop dependency to QGLWidget::convertToGLFormat().
static void convertToGLFormatHelper(QImage & dst, const QImage & img, GLenum texture_format)
{
    Q_ASSERT(dst.depth() == 32);
    Q_ASSERT(img.depth() == 32);

    if (dst.size() != img.size())
    {
        int target_width = dst.width();
        int target_height = dst.height();
        qreal sx = target_width / qreal(img.width());
        qreal sy = target_height / qreal(img.height());

        quint32 * dest = (quint32 *)dst.scanLine(0); // NB! avoid detach here
        uchar * srcPixels = (uchar *)img.scanLine(img.height() - 1);
        int sbpl = img.bytesPerLine();
        int dbpl = dst.bytesPerLine();

        int ix = int(0x00010000 / sx);
        int iy = int(0x00010000 / sy);

        quint32 basex = int(0.5 * ix);
        quint32 srcy = int(0.5 * iy);

        // scale, swizzle and mirror in one loop
        while (target_height--)
        {
            const uint * src = (const quint32 *)(srcPixels - (srcy >> 16) * sbpl);
            int srcx = basex;
            for (int x = 0; x < target_width; ++x)
            {
                dest[x] = qt_gl_convertToGLFormatHelper(src[srcx >> 16], texture_format);
                srcx += ix;
            }
            dest = (quint32 *)(((uchar *)dest) + dbpl);
            srcy += iy;
        }
    }
    else
    {
        const int width = img.width();
        const int height = img.height();
        const uint * p = (const uint *)img.scanLine(img.height() - 1);
        uint * q = (uint *)dst.scanLine(0);

#ifdef __MC_GLES__
        if (false)
        {
#else
        if (texture_format == GL_BGRA)
        {
#endif
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
            {
                // mirror + swizzle
                for (int i = 0; i < height; ++i)
                {
                    const uint * end = p + width;
                    while (p < end)
                    {
                        *q = ((*p << 24) & 0xff000000)
                          | ((*p >> 24) & 0x000000ff)
                          | ((*p << 8) & 0x00ff0000)
                          | ((*p >> 8) & 0x0000ff00);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            }
            else
            {
                const uint bytesPerLine = img.bytesPerLine();
                for (int i = 0; i < height; ++i)
                {
                    memcpy(q, p, bytesPerLine);
                    q += width;
                    p -= width;
                }
            }
        }
        else
        {
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
            {
                for (int i = 0; i < height; ++i)
                {
                    const uint * end = p + width;
                    while (p < end)
                    {
                        *q = (*p << 8) | ((*p >> 24) & 0xff);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            }
            else
            {
                for (int i = 0; i < height; ++i)
                {
                    const uint * end = p + width;
                    while (p < end)
                    {
                        *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            }
        }
    }
}

MCSurfaceManager::MCSurfaceManager()
{
}

MCGLColor getAverageColor(QImage image)
{
    std::map<int, int> colorCounts;
    const int scale = 8;
    for (int i = 0; i < image.width(); i++)
    {
        for (int j = 0; j < image.height(); j++)
        {
            if (((image.pixel(i, j) & 0xff000000) >> 24) > 128)
            {
                const int r = ((image.pixel(i, j) & 0x00ff0000) >> 16) / scale;
                const int g = ((image.pixel(i, j) & 0x0000ff00) >> 8) / scale;
                const int b = ((image.pixel(i, j) & 0x000000ff)) / scale;
                colorCounts[(r << 16) + (g << 8) + b]++;
            }
        }
    }
    int bestColor = 0;
    int bestCount = 0;
    for (auto && color : colorCounts)
    {
        if (color.second > bestCount)
        {
            bestCount = color.second;
            bestColor = color.first;
        }
    }
    return MCGLColor(
      static_cast<float>(((bestColor & 0xff0000) >> 16) * scale) / 256,
      static_cast<float>(((bestColor & 0x00ff00) >> 8) * scale) / 256,
      static_cast<float>((bestColor & 0x0000ff) * scale) / 256);
}

std::shared_ptr<MCSurface> MCSurfaceManager::createSurfaceFromImage(const MCSurfaceMetaData & data, QImage image)
{
    if (data.handle.size() == 0)
    {
        throw std::runtime_error("Cannot create surface with an empty handle!");
    }

    image = image.scaled(image.width() / data.sizeDivider, image.height() / data.sizeDivider);

    // Store original width of the image
    int origH = data.height.second ? data.height.first : image.height();
    int origW = data.width.second ? data.width.first : image.width();

    // Create material. Possible secondary textures are taken from surfaces
    // that are initialized before this surface.
    MCGLMaterialPtr material(new MCGLMaterial);
    material->setTexture(create2DTextureFromImage(data, image), 0);
    material->setTexture(data.handle2.length() ? surface(data.handle2)->material()->texture(0) : 0, 1);
    material->setTexture(data.handle3.length() ? surface(data.handle3)->material()->texture(0) : 0, 2);

    if (data.specularCoeff.second)
    {
        material->setSpecularCoeff(data.specularCoeff.first);
    }

    // Create a new MCSurface object
    const auto surface = std::make_shared<MCSurface>(data.handle, material, origW, origH, data.z0, data.z1, data.z2, data.z3);

    // Maybe better place for this could be in the material?
    surface->setColor(data.color);
    surface->setAverageColor(getAverageColor(image));

    createSurfaceCommon(surface, data);

    return surface;
}

void MCSurfaceManager::createSurfaceCommon(std::shared_ptr<MCSurface> surface, const MCSurfaceMetaData & data)
{
    // Enable alpha blend, if set
    surface->material()->setAlphaBlend(data.alphaBlend.second, data.alphaBlend.first.m_src, data.alphaBlend.first.m_dst);

    // Store MCSurface to map
    m_surfaceMap[data.handle] = surface;
}
#ifdef __MC_GLES__
static bool isPowerOfTwo(unsigned int x)
{
    return (x != 0) && !(x & (x - 1));
}

static unsigned int getNearestPowerOfTwo(unsigned int x)
{
    const std::vector<unsigned int> powers = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
    for (auto power : powers)
    {
        if (power > x)
        {
            return power;
        }
    }
    return powers.back();
}

static QImage forceToNearestPowerOfTwoImage(const MCSurfaceMetaData & data, const QImage & image)
{
    unsigned int width = image.width();
    unsigned int height = image.height();

    if (isPowerOfTwo(width) && isPowerOfTwo(height))
    {
        return image;
    }

    if (!isPowerOfTwo(width))
    {
        MCLogger().warning() << "Width of surface '" + data.handle + "' not power of two: " << width;
        width = getNearestPowerOfTwo(width);
        MCLogger().warning() << "    new value: " << width;
    }

    if (!isPowerOfTwo(height))
    {
        MCLogger().warning() << "Height of surface '" + data.handle + "' not power of two: " << height;
        height = getNearestPowerOfTwo(height);
        MCLogger().warning() << "    new value: " << height;
    }

    return image.scaled(width, height);
}
#endif
GLuint MCSurfaceManager::create2DTextureFromImage(const MCSurfaceMetaData & data, const QImage & image)
{
#ifdef __MC_GLES__
    QImage textureImage = forceToNearestPowerOfTwoImage(data, image);
#else
    QImage textureImage = image;
#endif

    // Take the maximum supported texture size into account
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    if (textureImage.width() > maxTextureSize && textureImage.height() > maxTextureSize)
    {
        textureImage = textureImage.scaled(maxTextureSize, maxTextureSize);
    }
    else if (textureImage.width() > maxTextureSize)
    {
        textureImage = textureImage.scaled(maxTextureSize, textureImage.height());
    }
    else if (textureImage.height() > maxTextureSize)
    {
        textureImage = textureImage.scaled(textureImage.width(), maxTextureSize);
    }

    // Flip if set active
    if (data.xAxisMirror || data.yAxisMirror)
    {
        textureImage = textureImage.mirrored(data.xAxisMirror, data.yAxisMirror);
    }

    // Ensure alpha channel
    textureImage = textureImage.convertToFormat(QImage::Format_ARGB32);

    // Apply alpha clamp if it was set (clear alpha)
    if (data.alphaClamp.second)
    {
        applyAlphaClamp(textureImage, static_cast<unsigned int>(255.0f * data.alphaClamp.first));
    }

    // Apply colorkey if it was set (set or clear alpha)
    if (data.colorKeySet)
    {
        applyColorKey(textureImage, data.colorKey.m_r, data.colorKey.m_g, data.colorKey.m_b);
    }

    QImage glFormattedImage(textureImage.width(), textureImage.height(), textureImage.format());
    convertToGLFormatHelper(glFormattedImage, textureImage, GL_RGBA);

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

    if (data.wrapS.second)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data.wrapS.first);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    }

    if (data.wrapT.second)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data.wrapT.first);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Edit image data using the information textureImage gives us
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glFormattedImage.width(), glFormattedImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, glFormattedImage.bits());

    return textureHandle;
}

void MCSurfaceManager::applyColorKey(QImage & textureImage, unsigned int r, unsigned int g, unsigned int b) const
{
    for (int i = 0; i < textureImage.width(); i++)
    {
        for (int j = 0; j < textureImage.height(); j++)
        {
            if (colorMatch(textureImage.pixel(i, j) & 0x000000ff, b, 2) && colorMatch((textureImage.pixel(i, j) & 0x0000ff00) >> 8, g, 2) && colorMatch((textureImage.pixel(i, j) & 0x00ff0000) >> 16, r, 2))
            {
                textureImage.setPixel(i, j, 0x00000000);
            }
            else
            {
                textureImage.setPixel(i, j, textureImage.pixel(i, j) | 0xff000000);
            }
        }
    }
}

void MCSurfaceManager::applyAlphaClamp(QImage & textureImage, unsigned int a) const
{
    for (int i = 0; i < textureImage.width(); i++)
    {
        for (int j = 0; j < textureImage.height(); j++)
        {
            if (((textureImage.pixel(i, j) >> 24) & 0x000000ff) < a)
            {
                textureImage.setPixel(i, j, 0x00000000);
            }
        }
    }
}

MCSurfaceManager::~MCSurfaceManager()
{
    // Delete OpenGL textures and Textures
    for (auto && iter : m_surfaceMap)
    {
        if (iter.second)
        {
            const auto surface = iter.second;
            for (size_t i = 0; i < MCGLMaterial::MAX_TEXTURES; i++)
            {
                GLuint dummyHandle1 = surface->material()->texture(i);
                glDeleteTextures(1, &dummyHandle1);
            }
        }
    }
}

void MCSurfaceManager::load(const std::string & configFilePath, const std::string & baseDataPath)
{
    MCSurfaceConfigLoader loader;

    // Parse the texture config file
    if (loader.load(configFilePath))
    {
        for (size_t i = 0; i < loader.surfaceCount(); i++)
        {
            const auto metaData = loader.surface(i);

            // Load the image and create a 2D texture. Due to possible Android asset URLs,
            // an explicit QFile-based loading is used instead of directly using QImage::loadFromFile().
            QString path = QString(baseDataPath.c_str()) + QDir::separator() + metaData.imagePath.c_str();
            path.replace("./", "");
            path.replace("//", "/");

            QFile imageFile(path);
            if (!imageFile.open(QIODevice::ReadOnly))
            {
                throw std::runtime_error("Cannot read file '" + path.toStdString() + "'");
            }

            QImage textureImage;
            textureImage.loadFromData(imageFile.readAll());
            createSurfaceFromImage(metaData, textureImage);
        }
    }
    else
    {
        // Throw an exception
        throw std::runtime_error("Parsing '" + configFilePath + "' failed!");
    }
}

std::shared_ptr<MCSurface> MCSurfaceManager::surface(const std::string & id) const
{
    // Try to find existing texture for the surface
    if (!m_surfaceMap.count(id))
    {
        throw std::runtime_error("Cannot find texture object for handle '" + id + "'");
    }

    // Yes: return handle for the texture
    return m_surfaceMap.find(id)->second;
}
