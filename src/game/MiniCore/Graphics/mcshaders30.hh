// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MCSHADERS30_HH
#define MCSHADERS30_HH

static const char * MCMasterVsh =
"#version 130\n"
"in vec3 inVertex;\n"
"in vec3 inNormal;\n"
"in vec2 inTexCoord;\n"
"in vec4 inColor;\n"
"uniform vec4 scale;\n"
"uniform vec4 color;\n"
"uniform mat4 vp;\n"
"uniform mat4 model;\n"
"uniform float fade;\n"
"uniform vec4 diffuseLightDir;\n"
"uniform vec4 diffuseLightColor;\n"
"uniform vec4 ambientLightColor;\n"
"out vec2 texCoord0;\n"
"out vec4 vColor;\n"
""
"void main()\n"
"{"
"    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
""
"    mat4 normalRot = mat4(mat3(model));\n"
"    float diffuseLightIntensity = dot(normalize(diffuseLightDir), normalRot * vec4(-inNormal, 1)) * diffuseLightColor.a;\n"
"    vColor = inColor * color * (\n"
"    vec4(ambientLightColor.rgb, 1.0) * ambientLightColor.a +\n"
"    vec4(diffuseLightColor.rgb, 1.0) * diffuseLightIntensity) * vec4(fade, fade, fade, 1.0);\n"
""
"    texCoord0 = inTexCoord;\n"
"}\n";

static const char * MCMasterFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec2 texCoord0;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 texColor = texture2D(tex0, texCoord0);\n"
""
"    if (texColor.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        fragColor = vColor * texColor;\n"
"    }\n"
"}\n";

static const char * MCMasterShadowVsh =
"#version 130\n"
""
"in vec3 inVertex;\n"
"in vec3 inNormal;\n"
"in vec2 inTexCoord;\n"
"in vec4 inColor;\n"
"uniform vec4 scale;\n"
"uniform vec4 color;\n"
"uniform mat4 vp;\n"
"uniform mat4 model;\n"
"out vec2 texCoord0;\n"
"out vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
"    vColor      = inColor * color;\n"
"    texCoord0   = inTexCoord;\n"
"}\n";

static const char * MCMasterShadowFsh =
"#version 130\n"
""
"uniform sampler2D tex0;\n"
"in vec2 texCoord0;\n"
"out vec4 fragColor;\n"
""
"void main(void)\n"
"{\n"
"    vec4 color = texture2D(tex0, texCoord0);\n"
"    if (color.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        fragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
"    }\n"
"}\n";

#endif // MCSHADERS30_HH
