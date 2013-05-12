// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#version 130

in vec3 inVertex;
in vec3 inNormal;
in vec4 inColor;

uniform mat4 mvp;
uniform float fade;

out mat2 rotationMatrix;
out vec4 vColor;

void main()
{
    // Simple transform
    gl_Position = mvp * vec4(inVertex, 1);
    
    // Copy the primary color
    vColor = inColor * fade;
    
    // Rotation matrix to rotate a point sprite (sin and cos delivered as normals)
    rotationMatrix = mat2(inNormal.x, inNormal.y, -inNormal.y, inNormal.x);
}

