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
in vec2 inTexCoord;
in vec4 inColor;

uniform vec4 pos;
uniform vec4 scale;
uniform vec4 color;
uniform mat4 mvp;

uniform float fade;

out vec2 texCoord0;
out vec4 vColor;

void main()
{
    // Normal MVP transform
    gl_Position = mvp * (vec4(inVertex, 1) * scale + pos);
    
    // Copy the primary color
    vColor = inColor * color * fade;

    // Copy texture coorinates
    texCoord0 = inTexCoord;
}

