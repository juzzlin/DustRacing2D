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

uniform sampler3D texture;
uniform float     fade;

void main(void)
{
    vec4 color = texture3D(texture, vec3(gl_TexCoord[0].st, 1.0));
    
    // Alpha test
    if (color.a < 0.1)
    {
        discard;
    }
    else if (color.r < 0.1 && color.g > 0.9 && color.r < 0.1)
    {
        discard;
    }
    else
    {
        gl_FragColor = color * fade * vec4(1.0, 0.95, 0.9, 1.0);
    }
}
