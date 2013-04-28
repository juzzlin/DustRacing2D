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

#ifndef MCSURFACEMANAGER_HH
#define MCSURFACEMANAGER_HH

#include <QImage>

#include <string>
#include <unordered_map>

#include "mcexception.hh"
#include "mcmacros.hh"
#include "mcsurfacemetadata.hh"

class MCSurface;

/*! Surface (texture) manager base class.
 *
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
 * <surfaces baseImagePath="./data/images/">
 *   <surface handle="Car1" image1="car1.png" w="16" h="16" xAxisMirror="1">
 *     <colorKey r="255" g="255" b="255"/>
 *   </surface>
 *   <surface handle="Car2" image1="car2.png" w="16" h="16">
 *     <alphaTest function="greater" threshold="0.5"/>
 *     <alphaBlend src="srcAlpha" dest="oneMinusSrcAlpha"/>
 *   </surface>
 *   <surface handle="WallMultiTexture" image1="wall.bmp" image2="wall2.bmp"/>
 *   <surface handle="Track" image1="track.bmp"/>
 *   <surface handle="Bazooka" image1="bazooka.jpg">
 *     <center x="10" y="5"/>
 *     <colorKey r="255" g="0" b="0"/>
 *   </surface>
 *   <surface handle="WINDOW_ICON" image1="logo_v2.bmp"/>
 * </surfaces>
 *
 */
class MCSurfaceManager
{
public:

    //! Constructor
    MCSurfaceManager();

    //! Destructor
    virtual ~MCSurfaceManager();

    //! Loads texture config from strBasePath using the given mapping file.
    //! \param configFilePath Path to the XML-based input file.
    //! \param baseDataPath The absolute search path for an image is
    //! baseDataPath + baseImagePath + fileName. baseImagePath and the fileName are
    //! defined in the input file.
    virtual void load(
        const std::string & configFilePath, const std::string & baseDataPath) throw (MCException);

    //! Returns a surface object associated with given strId.
    //! Corresponding OpenGL texture handle can be obtained
    //! by calling handle() of the resulting MCSurface.
    //! MCSurfaceManager will keep the ownership.
    //! \param handle Handle defined in the textures XML file.
    //! \return Reference to the corresponding MCSurface.
    //! \throws MCException on failure.
    MCSurface & surface(
        const std::string & handle) const throw (MCException);

    //! Creates an MCSurface containing a 2D OpenGL texture from a QImage + texture meta data.
    //! MCSurfaceManager keeps the ownership.
    MCSurface & createSurfaceFromImage(
        const MCSurfaceMetaData & data, const QImage & image) throw (MCException);

    //! Creates an MCSurface containing two 2D OpenGL textures from two QImages + texture meta data.
    //! MCSurfaceManager keeps the ownership.
    MCSurface & createSurfaceFromImages(
        const MCSurfaceMetaData & data, const QImage & image1, const QImage & image2) throw (MCException);

private:

    //! Apply given color key (set alpha values on / of).
    void applyColorKey(QImage & textureImage,
        MCUint r, MCUint g, MCUint b) const;

    //! Helper to create the actual OpenGL texture.
    GLuint create2DTextureFromImage(const MCSurfaceMetaData & data, const QImage & image);

    //! Map for resulting surface objects
    typedef std::unordered_map<std::string, MCSurface *> SurfaceHash;
    SurfaceHash m_surfaceMap;

    DISABLE_COPY(MCSurfaceManager);
    DISABLE_ASSI(MCSurfaceManager);
};

#endif // MCSURFACEMANAGER_HH
