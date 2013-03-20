// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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
in vec2 inTexCoord;

uniform vec4 pos;
uniform vec4 scale;
uniform mat4 mvp;

out vec2 texCoord0;

void main()
{
    mat4 translation = mat4(
        1.0,   0.0,   0.0,   0.0,
        0.0,   1.0,   0.0,   0.0,
        0.0,   0.0,   1.0,   0.0,
        pos.x, pos.y, pos.z, 1.0);

    // Normal MVP transform
    gl_Position = mvp * translation * (vec4(inVertex, 1) * scale);

    // Copy texture coorinates
    texCoord0 = inTexCoord;
}

