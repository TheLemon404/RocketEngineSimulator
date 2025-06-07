#version 450

layout(location = 0) in vec3 passNormal;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(passNormal, 1.0f);
}