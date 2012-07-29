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

attribute vec4 position;
attribute vec3 scale;
attribute float angle;

void main()
{
    float r = angle * 3.1415 / 180.0;
    mat4  m = mat4(
        cos(r),  sin(r), 0.0, 0.0,
        -sin(r), cos(r), 0.0, 0.0,
        0.0,     0.0,    1.0, 0.0,
        0.0,     0.0,    0.0, 1.0);

    // Normal MVP transform
    vec4 scaled = gl_Vertex;
    scaled.x    = gl_Vertex.x * scale.x;
    scaled.y    = gl_Vertex.y * scale.y;
    scaled.z    = gl_Vertex.z * scale.z;
    scaled.w    = 1.0;
    gl_Position = gl_ModelViewProjectionMatrix * (m * scaled + position);
    
    // Copy the promary color
    gl_FrontColor = gl_Color;

    // Copy texture coorinates    
    gl_TexCoord[0] = gl_MultiTexCoord0;
}

