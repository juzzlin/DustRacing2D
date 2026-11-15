// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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
#ifndef CARBODY_HPP
#define CARBODY_HPP

#include <MCGLMaterial>
#include <MCGLShaderProgram>
#include <MCMesh>
#include <MCVector2d>
#include <MCVector3d>

#include <vector>

/*! The body of a car as a grid mesh that dents and bends in collisions. The texture stretches
 *  with the grid and the normals follow the dents, so that the light shows them. */
class CarBody
{
public:
    /*! \param length Length of the car along its x-axis.
     *  \param width Width of the car along its y-axis.
     *  \param z Height of the body. */
    CarBody(MCGLMaterialPtr material, float length, float width, float z, MCGLShaderProgramPtr program, MCGLShaderProgramPtr shadowProgram);

    MCMeshPtr mesh() const;

    /*! Dents the body on the side that was hit.
     *  \param direction Direction of the collision impulse in the frame of the car. The impulse
     *  pushes the car away from the hit, so the body is hit from the opposite direction.
     *  \param damage Damage caused by the collision. */
    void deform(MCVector2dF direction, float damage);

    //! Repairs the body.
    void reset();

    //! Number of grid cells along the length and the width of the car.
    static constexpr int columns = 16;

    static constexpr int rows = 8;

    //! \return displacement of the given grid point from its rest position.
    const MCVector3dF & displacement(int column, int row) const;

private:
    MCVector3dF restLocation(int column, int row) const;

    MCVector3dF location(int column, int row) const;

    MCVector3dF normal(int column, int row) const;

    MCMesh::FaceVector buildFaces() const;

    void updateMesh();

    float m_length;

    float m_width;

    float m_z;

    std::vector<MCVector3dF> m_displacements;

    MCMeshPtr m_mesh;
};

#endif // CARBODY_HPP
