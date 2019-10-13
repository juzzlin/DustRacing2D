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

static const char * MCDefaultVsh =
  "#version 130\n"
  "in      vec3  inVertex;\n"
  "in      vec3  inNormal;\n"
  "in      vec2  inTexCoord;\n"
  "in      vec4  inColor;\n"
  "uniform vec4  scale = vec4(1, 1, 1, 1);\n"
  "uniform vec4  color = vec4(1, 1, 1, 1);\n"
  "uniform mat4  vp;\n"
  "uniform mat4  model;\n"
  "uniform vec4  dd = vec4(1, 1, 1, 1);\n"
  "uniform vec4  dc = vec4(1, 1, 1, 1);\n"
  "uniform float dCoeff = 1;\n"
  "uniform vec4  ac = vec4(1, 1, 1, 1);\n"
  "out     vec2  texCoord0;\n"
  "out     vec4  vColor;\n"
  ""
  "void main()\n"
  "{"
  "    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
  ""
  "    mat4 normalRot = mat4(mat3(model));\n"
  "    float di = dot(dd.xyz, (normalRot * vec4(-inNormal, 1)).xyz) * dc.a;\n"
  "    vColor = inColor * color * (\n"
  "        vec4(ac.rgb * ac.a, 1.0) +\n"
  "        vec4(dc.rgb * di * dCoeff, 1.0));\n"
  ""
  "    texCoord0 = inTexCoord;\n"
  "}\n";

static const char * MCDefaultVshSpecular =
  "#version 130\n"
  ""
  "attribute vec3  inVertex;\n"
  "attribute vec3  inNormal;\n"
  "attribute vec2  inTexCoord;\n"
  "attribute vec4  inColor;\n"
  "uniform   vec4  scale = vec4(1, 1, 1, 1);\n"
  "uniform   vec4  color = vec4(1, 1, 1, 1);\n"
  "uniform   mat4  vp;\n"
  "uniform   mat4  v;\n"
  "uniform   mat4  model;\n"
  "uniform   vec4  dd = vec4(1, 1, 1, 1);\n"
  "uniform   vec4  dc = vec4(1, 1, 1, 1);\n"
  "uniform   vec4  sd = vec4(1, 1, 1, 1);\n"
  "uniform   vec4  sc = vec4(1, 1, 1, 1);\n"
  "uniform   vec4  ac = vec4(1, 1, 1, 1);\n"
  "uniform   float sCoeff = 1;\n"
  "uniform   float dCoeff = 1;\n"
  "varying   vec2  texCoord0;\n"
  "varying   vec4  vColor;\n"
  ""
  "void main()\n"
  "{"
  "    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
  ""
  "    mat4 normalRot = mat4(mat3(model));\n"
  "    float di = dot(dd.xyz, (normalRot * vec4(-inNormal, 1)).xyz) * dc.a;\n"
  ""
  "    vec3 vNormalEye = (v * model * vec4(inNormal, 1)).xyz;\n"
  "    vec3 vVertexEye = (v * model * vec4(inVertex, 1) * scale).xyz;\n"
  ""
  "    vec3 eye = vec3(0, 0, 1);\n"
  "    vec3 pos = vVertexEye;\n"
  "    vec3 V   = normalize(eye - pos);\n"
  "    vec3 L   = normalize(-eye);\n"
  "    vec3 N   = normalize(-vNormalEye);\n"
  ""
  "    float si = max(0.0, pow(dot(reflect(L, N), V), sCoeff));\n"
  ""
  "    vColor = (inColor * color * (\n"
  "        vec4(ac.rgb * ac.a, 1.0) +\n"
  "        vec4(dc.rgb * di * dCoeff, 1.0)) + vec4(sc.xyz * si, 1.0));\n"
  ""
  "    texCoord0 = inTexCoord;\n"
  "}\n";

static const char * MCDefaultFsh =
  "#version 130\n"
  ""
  "uniform sampler2D tex0;\n"
  "in      vec2      texCoord0;\n"
  "in      vec4      vColor;\n"
  "out     vec4      fragColor;\n"
  ""
  "void main(void)\n"
  "{\n"
  "    vec4 texColor = texture2D(tex0, texCoord0);\n"
  "    if (texColor.a < 0.1)\n"
  "    {\n"
  "        discard;\n"
  "    }\n"
  "    else\n"
  "    {\n"
  "        fragColor = vColor * texColor;\n"
  "    }\n"
  "}\n";

static const char * MCDefaultShadowVsh =
  "#version 130\n"
  ""
  "in      vec3 inVertex;\n"
  "in      vec2 inTexCoord;\n"
  "uniform vec4 scale = vec4(1, 1, 1, 1);\n"
  "uniform mat4 vp;\n"
  "uniform mat4 model;\n"
  "out     vec2 texCoord0;\n"
  ""
  "void main()\n"
  "{\n"
  "    gl_Position = vp * model * (vec4(inVertex.xy, 0, 1) * scale);\n"
  "    texCoord0   = inTexCoord;\n"
  "}\n";

static const char * MCDefaultShadowFsh =
  "#version 130\n"
  ""
  "uniform sampler2D tex0;\n"
  "in      vec2      texCoord0;\n"
  "out     vec4      fragColor;\n"
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
  "        fragColor = vec4(0.0, 0.0, 0.0, 0.5);\n"
  "    }\n"
  "}\n";

static const char * MCDefaultTextVsh =
  "#version 130\n"
  ""
  "in      vec3  inVertex;\n"
  "in      vec3  inNormal;\n"
  "in      vec2  inTexCoord;\n"
  "in      vec4  inColor;\n"
  "uniform vec4  scale = vec4(1, 1, 1, 1);\n"
  "uniform vec4  color = vec4(1, 1, 1, 1);\n"
  "uniform mat4  vp;\n"
  "uniform mat4  model;\n"
  "out     vec2  texCoord0;\n"
  "out     vec4  vColor;\n"
  ""
  "void main()\n"
  "{\n"
  "    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
  "    vColor      = inColor * color;\n"
  "    texCoord0   = inTexCoord;\n"
  "}\n";

static const char * MCDefaultTextShadowFsh =
  "#version 130\n"
  ""
  "uniform sampler2D tex0;\n"
  "in      vec2      texCoord0;\n"
  "out     vec4      fragColor;\n"
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

static const char * MCDefaultFBOVsh =
  "#version 130\n"
  ""
  "in  vec3 inVertex;\n"
  "in  vec2 inTexCoord;\n"
  "out vec2 texCoord0;\n"
  ""
  "void main()\n"
  "{\n"
  "    gl_Position = vec4(inVertex, 1);\n"
  "    texCoord0 = inTexCoord;\n"
  "}\n";

static const char * MCDefaultFBOFsh =
  "#version 130\n"
  ""
  "uniform sampler2D tex0;\n"
  "in      vec2      texCoord0;\n"
  "out     vec4      fragColor;\n"
  ""
  "void main(void)\n"
  "{\n"
  "    fragColor = texture2D(tex0, texCoord0);\n"
  "}\n";

#endif // MCSHADERS30_HH
