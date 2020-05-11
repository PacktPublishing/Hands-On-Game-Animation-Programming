#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec4 weights;
in ivec4 joints;

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

uniform mat4 animated[120];

void main() {
    mat4 skin = animated[joints.x] * weights.x +
                animated[joints.y] * weights.y +
                animated[joints.z] * weights.z +
                animated[joints.w] * weights.w;
    gl_Position = projection * view * model * skin * vec4(position, 1.0);

    fragPos = vec3(skin * vec4(normal, 0));
    norm = vec3(model * skin* vec4(normal, 0));
    uv = texCoord;
}
