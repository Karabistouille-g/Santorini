#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // L'ordre est très important : P * V * M
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}