// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef MCSHADERS_HH
#define MCSHADERS_HH

static const char * MCDefaultVsh =
"#version 120\n"
""
"attribute vec3  inVertex;\n"
"attribute vec3  inNormal;\n"
"attribute vec2  inTexCoord;\n"
"attribute vec4  inColor;\n"
"uniform   vec4  scale;\n"
"uniform   vec4  color;\n"
"uniform   mat4  vp;\n"
"uniform   mat4  model;\n"
"uniform   float fade;\n"
"uniform   vec4  dd;\n"
"uniform   vec4  dc;\n"
"uniform   vec4  ac;\n"
"varying   vec2  texCoord0;\n"
"varying   vec4  vColor;\n"
""
"void main()\n"
"{"
"    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
""
"    mat4 normalRot = mat4(mat3(model));\n"
"    float di = dot(dd, normalRot * vec4(-inNormal, 1)) * dc.a;\n"
"    vColor = inColor * color * (\n"
"        vec4(ac.rgb, 1.0) * ac.a +\n"
"        vec4(dc.rgb, 1.0) * di) * vec4(fade, fade, fade, 1.0);\n"
""
"    texCoord0 = inTexCoord;\n"
"}\n";

static const char * MCDefaultVshSpecular =
"#version 120\n"
""
"attribute vec3  inVertex;\n"
"attribute vec3  inNormal;\n"
"attribute vec2  inTexCoord;\n"
"attribute vec4  inColor;\n"
"uniform   vec4  scale;\n"
"uniform   vec4  color;\n"
"uniform   mat4  vp;\n"
"uniform   mat4  v;\n"
"uniform   mat4  model;\n"
"uniform   float fade;\n"
"uniform   vec4  dd;\n"
"uniform   vec4  dc;\n"
"uniform   vec4  sd;\n"
"uniform   vec4  sc;\n"
"uniform   vec4  ac;\n"
"uniform   float sCoeff;\n"
"varying   vec2  texCoord0;\n"
"varying   vec4  vColor;\n"
""
"void main()\n"
"{"
"    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
""
"    mat4 normalRot = mat4(mat3(model));\n"
"    float di = dot(dd, normalRot * vec4(-inNormal, 1)) * dc.a;\n"
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
"        vec4(ac.rgb, 1.0) * ac.a +\n"
"        vec4(dc.rgb, 1.0) * di) + vec4(sc.xyz, 1.0) * si) * vec4(fade, fade, fade, 1.0);\n"
""
"    texCoord0 = inTexCoord;\n"
"}\n";

static const char * MCDefaultFsh =
"#version 120\n"
""
"uniform sampler2D tex0;\n"
"uniform vec4      dd;\n"
"uniform vec4      dc;\n"
"varying vec2      texCoord0;\n"
"varying vec4      vColor;\n"
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
"        gl_FragColor = vColor * texColor;\n"
"    }\n"
"}\n";

static const char * MCDefaultShadowVsh =
"#version 120\n"
""
"attribute vec3 inVertex;\n"
"attribute vec3 inNormal;\n"
"attribute vec2 inTexCoord;\n"
"attribute vec4 inColor;\n"
"uniform   vec4 scale;\n"
"uniform   vec4 color;\n"
"uniform   mat4 vp;\n"
"uniform   mat4 model;\n"
"varying   vec2 texCoord0;\n"
"varying   vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vp * model * (vec4(inVertex.x, inVertex.y, 0, 1) * scale);\n"
"    vColor      = inColor * color;\n"
"    texCoord0   = inTexCoord;\n"
"}\n";

static const char * MCDefaultShadowFsh =
"#version 120\n"
""
"uniform sampler2D texture;\n"
"varying vec2      texCoord0;\n"
""
"void main(void)\n"
"{\n"
"    vec4 color = texture2D(texture, texCoord0);\n"
"    if (color.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.5);\n"
"    }\n"
"}\n";

static const char * MCDefaultPointParticleVsh =
"#version 120\n"
""
"attribute vec3 inVertex;\n"
"attribute vec3 inNormal;\n"
"attribute vec4 inColor;\n"
"uniform mat4 vp;\n"
"uniform float fade;\n"
"uniform float pointSize;\n"
"varying vec4 rotationMatrixTegra3Hack;\n"
"varying vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vp * vec4(inVertex, 1);\n"
"    gl_PointSize = pointSize * inVertex.z;\n"
"    vColor = inColor * fade;\n"
""
"    // Rotation matrix to rotate a point sprite (sin and cos delivered as normals)\n"
"    // The matrix is delivered as a vec4, as on Nexus 7 gives error if mat2 is used:\n"
"    // L0007: Cannot find vertex shader varying for declared fragment shader varyingrotationMatrix\n"
"    rotationMatrixTegra3Hack = vec4(inNormal.x, inNormal.y, -inNormal.y, inNormal.x);\n"
"}\n";

static const char * MCDefaultPointParticleFsh =
"#version 120\n"
""
"uniform sampler2D tex0;\n"
"varying vec4 vColor;\n"
""
"void main(void)\n"
"{\n"
"    gl_FragColor = texture2D(tex0, gl_PointCoord) * vColor;\n"
"}\n";

static const char * MCDefaultPointParticleRotateFsh =
"#version 120\n"
""
"uniform sampler2D tex0;\n"
"varying vec4 rotationMatrixTegra3Hack;\n"
"varying vec4 vColor;\n"
""
"void main(void)\n"
"{\n"
"    mat2 rotationMatrix = mat2(rotationMatrixTegra3Hack.x, rotationMatrixTegra3Hack.y, rotationMatrixTegra3Hack.z, rotationMatrixTegra3Hack.w);\n"
"    vec4 texColor = texture2D(tex0, rotationMatrix * gl_PointCoord) * vColor;\n"
"    gl_FragColor = texColor;\n"
"}\n";

static const char * MCDefaultParticleFsh =
"#version 120\n"
""
"varying vec4 vColor;\n"
""
"void main(void)\n"
"{\n"
"    gl_FragColor = vColor;\n"
"}\n";

static const char * MCDefaultTextVsh =
"#version 120\n"
""
"attribute vec3 inVertex;\n"
"attribute vec3 inNormal;\n"
"attribute vec2 inTexCoord;\n"
"attribute vec4 inColor;\n"
"uniform vec4 scale;\n"
"uniform vec4 color;\n"
"uniform mat4 vp;\n"
"uniform mat4 model;\n"
"uniform float fade;\n"
"varying vec2 texCoord0;\n"
"varying vec4 vColor;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vp * model * (vec4(inVertex, 1) * scale);\n"
"    vColor      = inColor * color * fade;\n"
"    texCoord0   = inTexCoord;\n"
"}\n";

static const char * MCDefaultTextShadowFsh =
"#version 120\n"
""
"uniform sampler2D texture;\n"
"varying vec2 texCoord0;\n"
""
"void main(void)\n"
"{\n"
"    vec4 color = texture2D(texture, texCoord0);\n"
"    if (color.a < 0.1)\n"
"    {\n"
"        discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
"    }\n"
"}\n";

#endif // MCSHADERS_HH
