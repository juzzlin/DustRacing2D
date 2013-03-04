// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

#include "soundconfigloader.hpp"

#include <MCLogger>
#include <cassert>

namespace SFX {

SoundConfigLoader::SoundConfigLoader()
{
}

bool SoundConfigLoader::load(const std::string & filePath)
{
    QDomDocument doc;
    QFile file(filePath.c_str());
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    if (!doc.setContent(&file))
    {
        file.close();
        return false;
    }

    file.close();

    QDomElement root = doc.documentElement();
    if (root.nodeName() == "sounds")
    {
        const std::string baseSoundPath = root.attribute("baseSoundPath", "./").toStdString();

        QDomNode node = root.firstChild();
        while(!node.isNull())
        {
            const bool isSound = node.nodeName() == "sound";
            const bool isMusic = node.nodeName() == "music";

            if (isSound || isMusic)
            {
                SoundMetaDataPtr newData(new SoundMetaData);

                QDomElement tag = node.toElement();
                if(!tag.isNull())
                {
                    newData->handle   = tag.attribute("handle", "").toStdString();
                    newData->filePath =
                        baseSoundPath +
                        QDir::separator().toAscii() +
                        tag.attribute("file", "").toStdString();
                    newData->isMusic  = isMusic;
                }

                m_sounds.push_back(newData);
            }

            node = node.nextSibling();
        }
    }

    return true;
}

unsigned int SoundConfigLoader::soundCount() const
{
    return m_sounds.size();
}

const SoundMetaData & SoundConfigLoader::sound(unsigned int index) const
{
    assert(index < static_cast<unsigned int>(m_sounds.size()));
    assert(m_sounds.at(index));
    return *m_sounds.at(index);
}

} // namespace SFX
