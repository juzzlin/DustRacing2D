// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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


#ifndef SHADERSMATERIAL_H
#define SHADERSMATERIAL_H

// GLSL helpers shared by the ground shaders of all GL versions.
// Material coordinates are in material units: one unit is one repeat of a material texture.
#define MATERIAL_GLSL                                                                       \
  "// Sine-free hash so that it stays stable with large coordinates and low precision.\n"   \
  "float hash(vec2 p)\n"                                                                    \
  "{\n"                                                                                     \
  "    vec3 p3 = fract(vec3(p.xyx) * 0.1031);\n"                                            \
  "    p3 += dot(p3, p3.yzx + 33.33);\n"                                                    \
  "    return fract((p3.x + p3.y) * p3.z);\n"                                               \
  "}\n"                                                                                     \
  ""                                                                                        \
  "float valueNoise(vec2 p)\n"                                                              \
  "{\n"                                                                                     \
  "    vec2 i = floor(p);\n"                                                                \
  "    vec2 f = fract(p);\n"                                                                \
  "    f = f * f * (3.0 - 2.0 * f);\n"                                                      \
  "    return mix(mix(hash(i), hash(i + vec2(1.0, 0.0)), f.x),\n"                           \
  "               mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x), f.y);\n"    \
  "}\n"                                                                                     \
  ""                                                                                        \
  "// Smooth noise with features spanning a few tiles.\n"                                   \
  "float macroNoise(vec2 uv)\n"                                                             \
  "{\n"                                                                                     \
  "    return valueNoise(uv * 0.35) * 0.65 + valueNoise(uv * 1.3) * 0.35;\n"                \
  "}\n"                                                                                     \
  ""                                                                                        \
  "// Sample a repeating material so that the repetition is not visible.\n"                 \
  "vec4 sampleMaterial(sampler2D tex, vec2 uv)\n"                                           \
  "{\n"                                                                                     \
  "    vec4 detail = texture2D(tex, uv);\n"                                                 \
  "    vec4 rotated = texture2D(tex, mat2(0.8, -0.6, 0.6, 0.8) * uv * 0.61 + 0.37);\n"      \
  "    vec4 color = mix(detail, rotated, smoothstep(0.3, 0.7, macroNoise(uv)) * 0.6);\n"    \
  ""                                                                                        \
  "    // Large patches of varying brightness and warmth (sun-bleached, dry areas).\n"      \
  "    float shade = macroNoise(uv + 17.0);\n"                                              \
  "    float warmth = smoothstep(0.55, 0.85, macroNoise(uv * 0.7 - 41.0));\n"               \
  "    color.rgb *= 0.86 + 0.28 * shade;\n"                                                 \
  "    color.rgb = mix(color.rgb, color.rgb * vec3(1.12, 1.02, 0.78), warmth * 0.6);\n"     \
  "    return color;\n"                                                                     \
  "}\n"                                                                                     \
  ""                                                                                        \
  "// Track tile masks: transparent is asphalt, green is a material and other colors are\n"  \
  "// painted details such as edge lines.\n"                                                \
  "float isAsphalt(vec4 mask)\n"                                                            \
  "{\n"                                                                                     \
  "    return 1.0 - step(0.01, mask.a);\n"                                                  \
  "}\n"                                                                                     \
  ""                                                                                        \
  "float isMaterial(vec4 mask)\n"                                                           \
  "{\n"                                                                                     \
  "    return step(0.01, mask.a) * step(mask.r + mask.b, mask.g);\n"                        \
  "}\n"                                                                                     \
  ""                                                                                        \
  "// Fraction of mask samples around uv that are not of the same kind as the pixel itself.\n" \
  "// 0.0 means deep inside an area, values towards 1.0 mean close to its border.\n"        \
  "float edgeProximity(sampler2D mask, vec2 uv, float radius, bool asphalt)\n"              \
  "{\n"                                                                                     \
  "    float same = 0.0;\n"                                                                 \
  "    for (int i = 0; i < 8; i++)\n"                                                       \
  "    {\n"                                                                                 \
  "        float angle = float(i) * 0.785398;\n"                                            \
  "        vec2 dir = vec2(cos(angle), sin(angle)) * radius;\n"                             \
  "        vec4 far = texture2D(mask, uv + dir);\n"                                         \
  "        vec4 near = texture2D(mask, uv + dir * 0.5);\n"                                  \
  "        same += asphalt ? isAsphalt(far) + isAsphalt(near) : isMaterial(far) + isMaterial(near);\n" \
  "    }\n"                                                                                 \
  "    return 1.0 - same / 16.0;\n"                                                         \
  "}\n"                                                                                     \
  ""                                                                                        \
  "// Anti-aliased mask pixels mix a painted color with the green material key.\n"         \
  "// Returns the amount of material and stores the unmixed paint color.\n"                \
  "float unmixPaint(vec4 mask, out vec3 paint)\n"                                           \
  "{\n"                                                                                     \
  "    float material = clamp(mask.g - max(mask.r, mask.b), 0.0, 0.99);\n"                  \
  "    paint = (mask.rgb - vec3(0.0, material, 0.0)) / (1.0 - material);\n"                 \
  "    return material;\n"                                                                  \
  "}\n"                                                                                     \
  ""                                                                                        \
  "// Worn, dusty grass and a soft contact shadow next to the track.\n"                     \
  "vec4 weatherVerge(vec4 color, float edge, vec2 uv)\n"                                    \
  "{\n"                                                                                     \
  "    float wear = smoothstep(0.05, 0.6, edge * (0.5 + valueNoise(uv * 24.0)));\n"         \
  "    float luminance = dot(color.rgb, vec3(0.299, 0.587, 0.114));\n"                      \
  "    vec3 dirt = luminance * vec3(1.35, 1.12, 0.78);\n"                                   \
  "    color.rgb = mix(color.rgb, dirt, wear * 0.65);\n"                                    \
  "    color.rgb *= 1.0 - 0.3 * smoothstep(0.35, 0.9, edge);\n"                             \
  "    return color;\n"                                                                     \
  "}\n"                                                                                     \
  ""                                                                                        \
  "// Translucent grime and rubber on the asphalt along the track borders.\n"               \
  "vec4 asphaltGrime(float edge, vec2 uv)\n"                                                \
  "{\n"                                                                                     \
  "    float amount = edge * (0.6 + 0.4 * valueNoise(uv * 16.0));\n"                        \
  "    return vec4(0.06, 0.05, 0.04, smoothstep(0.0, 0.7, amount) * 0.45);\n"                \
  "}\n"                                                                                     \
  ""                                                                                        \
  "// Width of the verge and grime bands in tile texture coordinates.\n"                    \
  "const float edgeRadius = 0.05;\n"

#endif // SHADERSMATERIAL_H
