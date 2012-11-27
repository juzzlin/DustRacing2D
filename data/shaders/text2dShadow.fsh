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

uniform sampler2D texture;

in vec2 texCoord0;

out vec4 fragColor;

void main(void)
{
    vec4 color = texture2D(texture, texCoord0);
    if (color.a < 0.1)
    {
        discard;
    }
    else
    {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
