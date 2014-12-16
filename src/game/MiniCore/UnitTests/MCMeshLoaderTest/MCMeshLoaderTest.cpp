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

#include "MCMeshLoaderTest.hpp"
#include "../Graphics/mcmesh.hh"

const char * TEST_DATA_CUBE =
"# Wavefront OBJ file\n"
"# Exported by Misfit Model 3D 1.3.7\n"
"# Thu Jan 17 20:11:34 2013\n"
"\n"
"# 8 Vertices\n"
"v -0.5 -0.5  0.5\n"
"  v  0.5 -0.5  0.5\n"
"v -0.5  0.5  0.5\n"
"v  0.5  0.5  0.5\n"
"v  0.5 -0.5 -0.5\n"
"v -0.5 -0.5 -0.5\n"
"v  0.5  0.5 -0.5\n"
"v -0.5  0.5 -0.5\n"
"\n"
"# 36 Texture Coordinates\n"
"vt 1.0 0.0\n"
"vt 0.0 0.0\n"
"vt 0.0 1.0\n"
"vt 1.0 0.0\n"
"vt 0.0 0.0\n"
"vt 0.0 1.0\n"
"vt 1.0 0.0\n"
"vt 0.0 0.0\n"
"vt 0.0 1.0\n"
"vt 1.0 0.0\n"
"vt 0.0 0.0\n"
"vt 0.0 1.0\n"
"vt 0.0 1.0\n"
"vt 0.0 0.0\n"
"vt 1.0 0.0\n"
"vt 0.0 1.0\n"
"vt 0.0 0.0\n"
"vt 1.0 0.0\n"
"vt 0.0 1.0\n"
"vt 0.0 0.0\n"
"vt 1.0 0.0\n"
"vt 0.0 1.0\n"
"vt 0.0 0.0\n"
"vt 1.0 0.0\n"
"vt 0.0 1.0\n"
"vt 0.0 0.0\n"
"vt 1.0 0.0\n"
"vt 0.0 1.0\n"
"vt 0.0 0.0\n"
"vt 1.0 0.0\n"
"vt 0.0 1.0\n"
"vt 0.0 0.0\n"
"vt 1.0 0.0\n"
"vt 0.0 1.0\n"
"vt 0.0 0.0\n"
"vt 1.0 0.0\n"
"\n"
"# 36 Vertex Normals\n"
"vn 0.0 0.0 1.0\n"
"vn 0.0 0.0 1.0\n"
"vn 0.0 0.0 1.0\n"
"vn 0.0 0.0 1.0\n"
"vn 0.0 0.0 1.0\n"
"vn 0.0 0.0 1.0\n"
"vn 0.0 0.0 -1.0\n"
"vn 0.0 0.0 -1.0\n"
"vn 0.0 0.0 -1.0\n"
"vn 0.0 0.0 -1.0\n"
"vn 0.0 0.0 -1.0\n"
"vn 0.0 0.0 -1.0\n"
"vn -1.0 0.0 0.0\n"
"vn -1.0 0.0 0.0\n"
"vn -1.0 0.0 0.0\n"
"vn -1.0 0.0 0.0\n"
"vn -1.0 0.0 0.0\n"
"vn -1.0 0.0 0.0\n"
"vn 1.0 0.0 0.0\n"
"vn 1.0 0.0 0.0\n"
"vn 1.0 0.0 0.0\n"
"vn 1.0 0.0 0.0\n"
"vn 1.0 0.0 0.0\n"
"vn 1.0 0.0 0.0\n"
"vn 0.0 1.0 0.0\n"
"vn 0.0 1.0 0.0\n"
"vn 0.0 1.0 0.0\n"
"vn 0.0 1.0 0.0\n"
"vn 0.0 1.0 0.0\n"
"vn 0.0 1.0 0.0\n"
"vn 0.0 -1.0 0.0\n"
"vn 0.0 -1.0 0.0\n"
"vn 0.0 -1.0 0.0\n"
"vn 0.0 -1.0 0.0\n"
"vn 0.0 -1.0 0.0\n"
"vn 0.0 -1.0 0.0\n"
"\n"
"# 12 Ungrouped triangles\n"
"\n"
"o ungrouped\n"
"g ungrouped\n"
"\n"
"f 1/1/1 2/2/2 3/3/3\n"
"f 2/4/4 4/5/5 3/6/6\n"
"f 5/7/7 6/8/8 7/9/9\n"
"f 6/10/10 8/11/11 7/12/12\n"
"f 3/13/13 6/14/14 1/15/15\n"
"f 3/16/16 8/17/17 6/18/18\n"
"f 7/19/19 2/20/20 5/21/21\n"
"f 7/22/22 4/23/23 2/24/24\n"
"f 3/25/25 7/26/26 8/27/27\n"
"f 3/28/28 4/29/29 7/30/30\n"
"f 2/31/31 6/32/32 5/33/33\n"
"f 2/34/34 1/35/35 6/36/36\n";

MCMeshLoaderTest::MCMeshLoaderTest()
{
}

void MCMeshLoaderTest::testSimple()
{
    QString testData(TEST_DATA_CUBE);
    QTextStream testStream(&testData);
    m_dut.readStream(testStream);

    QVERIFY(m_dut.vertices().size() == 8);
    QVERIFY(m_dut.textureCoords().size() == 36);
    QVERIFY(m_dut.normals().size() == 36);
}

void MCMeshLoaderTest::testFace()
{
    QString testData(TEST_DATA_CUBE);
    QTextStream testStream(&testData);
    m_dut.readStream(testStream);

    QVERIFY(m_dut.faces().size() == 12);

    MCMesh::Face face0 = m_dut.faces().at(0);
    QVERIFY(face0.vertices.size() == 3);
    QVERIFY(qFuzzyCompare(face0.vertices.at(0).x, -0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(0).y, -0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(0).z,  0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(0).u,  1.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(0).v,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(0).i,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(0).j,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(0).k,  1.0f));

    QVERIFY(qFuzzyCompare(face0.vertices.at(1).x,  0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(1).y, -0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(1).z,  0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(1).u,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(1).v,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(1).i,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(1).j,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(1).k,  1.0f));

    QVERIFY(qFuzzyCompare(face0.vertices.at(2).x, -0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(2).y,  0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(2).z,  0.5f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(2).u,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(2).v,  1.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(2).i,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(2).j,  0.0f));
    QVERIFY(qFuzzyCompare(face0.vertices.at(2).k,  1.0f));

    MCMesh::Face face11 = m_dut.faces().at(11);
    QVERIFY(face11.vertices.size() == 3);
    QVERIFY(qFuzzyCompare(face11.vertices.at(0).x,  0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(0).y, -0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(0).z,  0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(0).u,  0.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(0).v,  1.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(0).i,  0.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(0).j, -1.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(0).k,  0.0f));

    QVERIFY(qFuzzyCompare(face11.vertices.at(1).x, -0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(1).y, -0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(1).z,  0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(1).u,  0.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(1).v,  0.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(1).i,  0.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(1).j, -1.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(1).k,  0.0f));

    QVERIFY(qFuzzyCompare(face11.vertices.at(2).x, -0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(2).y, -0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(2).z, -0.5f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(2).u,  1.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(2).v,  0.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(2).i,  0.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(2).j, -1.0f));
    QVERIFY(qFuzzyCompare(face11.vertices.at(2).k,  0.0f));
}

QTEST_MAIN(MCMeshLoaderTest)
