// Author: Jake Rieger
// Created: 11/25/2024.
//

#pragma once

#include <Types.hpp>

namespace Xen::Shaders {
    namespace SpriteShader {
        static cstr Vertex = R""(#version 460 core
layout (location = 0) in vec4 aVertex;
uniform mat4 uMVP;
out vec2 TexCoord;

void main() {
    vec2 position = aVertex.xy;
    vec2 texCoord = aVertex.zw;
    gl_Position = uMVP * vec4(position, 0.0, 1.0);
    TexCoord = texCoord;
}
)"";

        static cstr Fragment = R""(#version 460 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D uSprite;

void main() {
    FragColor = texture(uSprite, TexCoord);
}
)"";
    }  // namespace SpriteShader
}  // namespace Xen::Shaders