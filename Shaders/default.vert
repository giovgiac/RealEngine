/**
 * default.vert
 *
 * Todos os direitos reservados.
 *
 */

#version 450 core

layout(set = 0, binding = 0) uniform transforms {
    mat4 model;
    mat4 view;
    mat4 proj;
} transform;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoords;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTexCoords;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    // Set Vertex Position
    gl_Position = transform.proj * transform.view * transform.model * vec4(position, 1.0);

    // Pass To Fragment Shader
    fragColor = color;
    fragTexCoords = texCoords;
}
