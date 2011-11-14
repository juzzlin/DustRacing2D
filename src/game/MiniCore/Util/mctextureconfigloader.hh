// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCTEXTURECONFIGLOADER_HH
#define MCTEXTURECONFIGLOADER_HH

#include <QString>
#include <QVector>

#include <GL/gl.h> // For GLenum.

class MCTextureData;

//! Manages the texture config loading.
class MCTextureConfigLoader
{
public:

    //! Constructor.
    MCTextureConfigLoader();

    //! Destructor.
    ~MCTextureConfigLoader();

    //! Set file path.
    void setConfigPath(QString filePath);

    //! Load all tracks found in the current filePath.
    //! \return true if succeeded.
    bool loadTextures();

    //! Get texture count.
    unsigned int textures() const;

    //! Get a texture of given index.
    MCTextureData * texture(unsigned int index) const;

private:

    //! Convert alpha test function string to enum.
    GLenum alphaFunctionStringToEnum(QString function) const;
    QString m_filePath;
    QVector<MCTextureData *> m_textures;
};

#endif // MCTEXTURECONFIGLOADER_HH
