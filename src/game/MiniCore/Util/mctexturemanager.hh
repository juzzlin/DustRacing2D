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

#ifndef MCTEXTUREMANAGER_HH
#define MCTEXTUREMANAGER_HH

#include <QImage>
#include <QString>
#include <QHash>

#include "../Core/mcexception.hh"
#include "../Core/mcmacros.hh"
#include "mctexturedata.hh"

class MCSurface;

/*! Texture manager base class.
 *
 * This is an OpenGL texture manager class.
 * It loads texture images (all formats supported by QImage) listed in a special mapping
 * file and stores the OpenGL textures with original image dimensions using an
 * internal texture wrapper class (MCSurface) designed for 2D-games.
 *
 * Textures are converted into GL_RGBA-format using the specified colorkey to mask out pixels
 * intended to be invisible. Dimensions are forced to the nearest power of 2.
 *
 * Texture surfaces will also be flipped about X-axis if desired.
 *  
 * MCSurface objects can be accessed via handles specified in the XML-based mapping file.
 *
 * Example mapping file:
 *
 * <?xml version="1.0"?>
 *
 * <textures version="1.0">
 *   <default_path value="./data/images/"/>
 *   <x_axis_mirror value=1/>
 *   <colorkey r=255 g=255 b=255/>
 *   <texture handle="BALL_GREEN" file="ball_green.png" w="16" h="16"/>
 *   <texture handle="BALL_BLUE"  file="ball_blue.bmp"/>
 *   <texture handle="BALL_BROWN" file="ball_brown.bmp"/>
 *   <texture handle="BAZOOKA"    file="bazooka.jpg" x="10" y="5"/>
 *   <colorkey r=255 g=0 b=0/>
 *   <x_axis_mirror value=0/>
 *   <texture handle="WINDOW_ICON" file="logo_v2.bmp"/>
 * </textures>
 *
 * Note: there can be multiple default_path, colorkey and x_axis_mirror
 *       settings, if needed.
 *
 */
class MCTextureManager
{
public:

    //! Constructor
    MCTextureManager();

    //! Destructor
    virtual ~MCTextureManager();

    //! Loads texture config from strBasePath using the given mapping file strFile.
    //! \param filePath Path to the XML-based input file.
    //! \param baseDataPath The absolute search path for an image is
    //! baseDataPath + default_path + file. default_path and file are
    //! defined in the input file.
    virtual void load(const QString & filePath, const QString & baseDataPath) throw (MCException);

    //! Returns a surface object associated with given strId.
    //! Corresponding OpenGL texture handle can be obtained
    //! by calling handle() of the resulting MCSurface.
    //! MCTextureManager will keep the ownership.
    //! \param handle Handle defined in the textures XML file.
    //! \return Pointer to the corresponding MCSurface.
    //! \throws MCException on failure.
    MCSurface * surface(const QString & handle) const throw (MCException);

private:

    //! Disable copy constructor
    DISABLE_COPY(MCTextureManager);

    //! Disable assignment
    DISABLE_ASSI(MCTextureManager);

    //! Creates an OpenGL texture from a QImage + texture meta data
    void createGLTextureFromImage(const MCTextureData & data, const QImage & image);

    //! Creates a scaled image with dimensions forced to the nearest power
    //! of two.
    QImage createNearest2PowNImage(const QImage & image);

    //! Map for resulting surface objects
    typedef QHash<QString, MCSurface *> TextureHash;
    TextureHash m_mapTextures;
};

#endif // MCTEXTUREMANAGER_HH
