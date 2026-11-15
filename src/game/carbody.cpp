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
#include "carbody.hpp"

#include <algorithm>
#include <cmath>

namespace {

//! Depth of a dent in world units per damage, and the maximum depth of a single dent.
const float depthPerDamage = 0.12f;
const float maxDentDepth = 3.0f;

//! Radius of a dent grows with the damage.
const float minDentRadius = 5.0f;
const float dentRadiusPerDamage = 0.4f;
const float maxDentRadius = 14.0f;

//! Dents also sink the body a bit.
const float sinkFactor = 0.25f;

//! Total displacement of a point is limited, so that the body never folds over itself.
const float maxDisplacement = 5.0f;
const float maxSink = 2.0f;

} // namespace

CarBody::CarBody(MCGLMaterialPtr material, float length, float width, float z, MCGLShaderProgramPtr program, MCGLShaderProgramPtr shadowProgram)
  : m_length(length)
  , m_width(width)
  , m_z(z)
  , m_displacements(static_cast<size_t>((columns + 1) * (rows + 1)))
{
    m_mesh = std::make_shared<MCMesh>("carBody", buildFaces(), material);
    m_mesh->setShaderProgram(program);
    m_mesh->setShadowShaderProgram(shadowProgram);
}

MCMeshPtr CarBody::mesh() const
{
    return m_mesh;
}

const MCVector3dF & CarBody::displacement(int column, int row) const
{
    return m_displacements.at(static_cast<size_t>(row * (columns + 1) + column));
}

MCVector3dF CarBody::restLocation(int column, int row) const
{
    return { -m_length / 2 + m_length * static_cast<float>(column) / columns, -m_width / 2 + m_width * static_cast<float>(row) / rows, m_z };
}

MCVector3dF CarBody::location(int column, int row) const
{
    return restLocation(column, row) + displacement(column, row);
}

MCVector3dF CarBody::normal(int column, int row) const
{
    const auto alongLength = location(std::min(column + 1, columns), row) - location(std::max(column - 1, 0), row);
    const auto alongWidth = location(column, std::min(row + 1, rows)) - location(column, std::max(row - 1, 0));
    return (alongLength % alongWidth).normalized();
}

void CarBody::deform(MCVector2dF direction, float damage)
{
    if (direction.isZero() || damage <= 0)
    {
        return;
    }

    direction.normalize();

    // The point on the outline of the car where the hit came from
    const MCVector2dF towardsHit { -direction.i(), -direction.j() };
    const float distance = std::min(
      std::fabs(towardsHit.i()) > 0 ? m_length / 2 / std::fabs(towardsHit.i()) : m_length,
      std::fabs(towardsHit.j()) > 0 ? m_width / 2 / std::fabs(towardsHit.j()) : m_width);
    const MCVector2dF hitPoint { towardsHit * distance };

    const float depth = std::min(depthPerDamage * damage, maxDentDepth);
    const float radius = std::min(minDentRadius + dentRadiusPerDamage * damage, maxDentRadius);

    for (int row = 0; row <= rows; row++)
    {
        for (int column = 0; column <= columns; column++)
        {
            const auto rest = restLocation(column, row);
            const float squaredDistance = (MCVector2dF { rest } - hitPoint).lengthSquared();
            const float weight = std::exp(-squaredDistance / (radius * radius));

            auto && offset = m_displacements.at(static_cast<size_t>(row * (columns + 1) + column));
            MCVector2dF horizontal { MCVector2dF { offset } + direction * (depth * weight) };
            if (const float length = horizontal.length(); length > maxDisplacement)
            {
                horizontal *= maxDisplacement / length;
            }

            const float sink = std::max(offset.k() - depth * sinkFactor * weight, -maxSink);
            offset = { horizontal.i(), horizontal.j(), sink };
        }
    }

    updateMesh();
}

void CarBody::reset()
{
    std::fill(m_displacements.begin(), m_displacements.end(), MCVector3dF {});
    updateMesh();
}

void CarBody::updateMesh()
{
    m_mesh->setFaces(buildFaces());
}

MCMesh::FaceVector CarBody::buildFaces() const
{
    const auto vertex = [this](int column, int row) {
        const auto point = location(column, row);
        const auto n = normal(column, row);
        MCMesh::Face::Vertex result;
        result.x = point.i();
        result.y = point.j();
        result.z = point.k();
        // The texture stays attached to the grid, so it stretches with the dents
        result.u = static_cast<float>(column) / columns;
        result.v = static_cast<float>(row) / rows;
        result.i = n.i();
        result.j = n.j();
        result.k = n.k();
        return result;
    };

    MCMesh::FaceVector faces;
    for (int row = 0; row < rows; row++)
    {
        for (int column = 0; column < columns; column++)
        {
            MCMesh::Face lower;
            lower.vertices = { vertex(column, row), vertex(column + 1, row), vertex(column + 1, row + 1) };
            faces.push_back(lower);

            MCMesh::Face upper;
            upper.vertices = { vertex(column, row), vertex(column + 1, row + 1), vertex(column, row + 1) };
            faces.push_back(upper);
        }
    }

    return faces;
}
