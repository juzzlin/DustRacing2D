// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCTEXTUREMANAGERIMPL_HH
#define MCTEXTUREMANAGERIMPL_HH

//! Implementation class for MCTextureManager
class MCTextureManagerImpl
{
    //! Constructor.
    MCTextureManagerImpl();

    //! Destructor.
    ~MCTextureManagerImpl();

    //! Creates an OpenGL texture from a QImage + texture meta data
    void createGLTextureFromImage(const MCTextureData & data, const QImage & image);

    //! Apply given color key (set alpha values on / off).
    void applyColorKey(QImage & textureImage,
        MCUint r, MCUint g, MCUint b) const;

    //! Creates a scaled image with dimensions forced to the nearest power
    //! of two.
    QImage createNearest2PowNImage(const QImage & image);

    //! Map for resulting surface objects
    typedef std::unordered_map<std::string, MCSurface *> SurfaceHash;
    SurfaceHash surfaceMap;

    friend class MCTextureManager;
};

#endif // MCTEXTUREMANAGERIMPL_HH
