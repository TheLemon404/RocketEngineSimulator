#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

layout(location = 0) out vec3 passNormal;

void main() {
    gl_Position = projection * view * transform * vec4(inPosition, 1.0);
    passNormal = inNormal;
}