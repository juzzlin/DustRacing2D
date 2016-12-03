// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcmeshloader.hh"

#include <QFile>
#include <QString>
#include <QStringList>
#include <cassert>

MCMeshLoader::MCMeshLoader()
{
    m_keyToFunctionMap["v"]  = std::bind(&MCMeshLoader::parseV,  this, std::placeholders::_1);
    m_keyToFunctionMap["vt"] = std::bind(&MCMeshLoader::parseVT, this, std::placeholders::_1);
    m_keyToFunctionMap["vn"] = std::bind(&MCMeshLoader::parseVN, this, std::placeholders::_1);
    m_keyToFunctionMap["f"]  = std::bind(&MCMeshLoader::parseF,  this, std::placeholders::_1);
}

bool MCMeshLoader::load(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    return readStream(in);
}

bool MCMeshLoader::readStream(QTextStream & stream)
{
    m_v.clear();
    m_vn.clear();
    m_vt.clear();

    m_faces.clear();

    QString line = stream.readLine();
    while (!line.isNull())
    {
        processLine(line);
        line = stream.readLine();
    }

    return true;
}

void MCMeshLoader::processLine(QString line)
{
    line.remove(QRegExp("^\\s+"));
    if (!line.startsWith('#'))
    {
        const QString key = line.split(QRegExp("\\s+")).at(0);
        auto iter = m_keyToFunctionMap.find(key);
        if (iter != m_keyToFunctionMap.end())
        {
            iter->second(line);
        }
    }
}

void MCMeshLoader::parseV(QString line)
{
    const QStringList atoms = line.split(QRegExp("\\s+"));
    V vertex = { atoms.at(1).toFloat(), atoms.at(2).toFloat(), atoms.at(3).toFloat() };
    m_v.push_back(vertex);
}

void MCMeshLoader::parseVN(QString line)
{
    const QStringList atoms = line.split(QRegExp("\\s+"));
    VN normal = { atoms.at(1).toFloat(), atoms.at(2).toFloat(), atoms.at(3).toFloat() };
    m_vn.push_back(normal);
}

void MCMeshLoader::parseVT(QString line)
{
    const QStringList atoms = line.split(QRegExp("\\s+"));
    VT tcoord = { atoms.at(1).toFloat(), atoms.at(2).toFloat() };
    m_vt.push_back(tcoord);
}

void MCMeshLoader::parseF(QString line)
{
    MCMesh::Face face;
    QStringList vertices = line.split(QRegExp("\\s+"));
    vertices.removeFirst();
    for (QString vertex : vertices)
    {
        const QStringList indices = vertex.split('/');
        assert(indices.size() == 3);

        // Location
        MCMesh::Face::Vertex faceVertex;
        const unsigned int vIndex = indices.at(0).toInt() - 1;
        assert(vIndex < m_v.size());
        faceVertex.x = m_v.at(vIndex).x;
        faceVertex.y = m_v.at(vIndex).y;
        faceVertex.z = m_v.at(vIndex).z;

        // Texture coordinate
        const unsigned int vtIndex = indices.at(1).toInt() - 1;
        assert(vtIndex < m_vt.size());
        faceVertex.u = m_vt.at(vtIndex).u;
        faceVertex.v = m_vt.at(vtIndex).v;

        // Normal
        const unsigned int vnIndex = indices.at(2).toInt() - 1;
        assert(vnIndex < m_vn.size());
        faceVertex.i = m_vn.at(vnIndex).x;
        faceVertex.j = m_vn.at(vnIndex).y;
        faceVertex.k = m_vn.at(vnIndex).z;

        face.vertices.push_back(faceVertex);
    }

    m_faces.push_back(face);
}

const MCMesh::FaceVector & MCMeshLoader::faces() const
{
    return m_faces;
}

const std::vector<MCMeshLoader::V> & MCMeshLoader::vertices() const
{
    return m_v;
}

const std::vector<MCMeshLoader::VN> & MCMeshLoader::normals() const
{
    return m_vn;
}

const std::vector<MCMeshLoader::VT> & MCMeshLoader::textureCoords() const
{
    return m_vt;
}
