/**
 * default.frag
 *
 * Todos os direitos reservados.
 *
 */

#version 450 core

layout (set = 0, binding = 1) uniform sampler2D textureSampler;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoords;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = texture(textureSampler, fragTexCoords);
}
