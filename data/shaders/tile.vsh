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
attribute float sin1;
attribute float cos1;

void main()
{
    mat4 m = mat4(
        cos1,       sin1,       0.0,        0.0,
        -sin1,      cos1,       0.0,        0.0,
        0.0,        0.0,        1.0,        0.0,
        position.x, position.y, position.z, 1.0);

    // Normal MVP transform
    gl_Position = gl_ModelViewProjectionMatrix * m * gl_Vertex;
    
    // Copy the primary color
    gl_FrontColor = gl_Color;

    // Copy texture coorinates
    gl_TexCoord[0] = gl_MultiTexCoord0;
}

