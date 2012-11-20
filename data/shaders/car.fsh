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

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float     fade;

void main(void)
{
    vec4 color = texture2D(texture0, gl_TexCoord[0].st);
    vec4 sky   = texture2D(texture1, gl_TexCoord[1].st);
    
    // Alpha test
    if (color.a < 0.1)
    {
        discard;
    }
    else
    {
        // Sky reflection
        vec4 ambient = fade * vec4(1.0, 0.95, 0.9, 1.0);
        if (color.r < 0.75 && color.g > 0.75 && color.b < 0.75)
        {
            gl_FragColor = sky * 0.75 * color.g * ambient;
        }
        else
        {
            gl_FragColor = color * ambient;
        }
    }
}

